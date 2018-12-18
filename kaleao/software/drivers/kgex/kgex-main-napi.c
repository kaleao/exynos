#include "kgex.h"

//#define DEBUG_FUNCTIONS


static struct net_device *net_dev_tbl[KGEX_MAX_NET_DEVS] = {NULL, NULL, NULL, NULL};
static uint32_t num_net_devs = 0;

int weight = KGEX_NAPI_WEIGHT;
module_param(weight, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(weight, "NAPI weight for driver. Default is 64");

int jumbo = 0;
module_param(jumbo, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(jumbo, "Enable Jumbo Ethernet Frame support.");

int tx_poll = 0;
module_param(tx_poll, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(tx_poll, "Use polling for TX.");

int rx_poll = 0;
module_param(rx_poll, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(rx_poll, "Use polling for RX.");

int tx_coalesce = DMA_DFT_TX_THRESHOLD;
module_param(tx_coalesce, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(tx_coalesce, "Ethernet frame coalescing count for TX.");

int rx_coalesce = DMA_DFT_RX_THRESHOLD;
module_param(rx_coalesce, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(rx_coalesce, "Ethernet frame coalescing count for RX.");

int ex_nr = 1;
module_param(ex_nr, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(ex_nr, "Exynos number [0,1,2,3].");


unsigned char network_fpga_mac_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x69};
unsigned char exynos_0_mac_A_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x64};
unsigned char exynos_1_mac_A_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x65};
unsigned char exynos_2_mac_A_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x66};
unsigned char exynos_3_mac_A_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x67};
unsigned char storage_fpga_mac_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x68};

unsigned char exynos_0_mac_B_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x04, 0x64};
unsigned char exynos_1_mac_B_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x04, 0x65};
unsigned char exynos_2_mac_B_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x04, 0x66};
unsigned char exynos_3_mac_B_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x04, 0x67};


unsigned char exynos_0_mac_C_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0xC8};
unsigned char exynos_1_mac_C_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0xC9};
unsigned char exynos_2_mac_C_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0xCA};
unsigned char exynos_3_mac_C_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0xCB};


unsigned char dest_mac_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x68};


static struct of_device_id kgex_match[] = {
	{ .compatible = "kmax,kmax-eth10g-1.00.a", },		        	
	{},	
};

MODULE_DEVICE_TABLE(of, kgex_match);

/******** Function Prototypes *******************************************/
//   Platform Driver Functions
int kgex_init (struct net_device **dev_out);
static int kgex_remove(struct platform_device *op);
static int kgex_probe(struct platform_device *op);

static struct platform_driver kgex_driver = {
	.probe		= kgex_probe,
	.remove		= kgex_remove,
	//.shutdown	= kgex_shutdown,
	.driver = {
		.name	= "kgex",
		.owner	= THIS_MODULE,
		.of_match_table = kgex_match,
	},
};
module_platform_driver(kgex_driver);

//   Network Device Functions
int kgex_open(struct net_device *dev);
int kgex_open_nic_B(struct net_device *dev);
int kgex_stop(struct net_device *dev);
int kgex_stop_nic_B(struct net_device *dev);
static int kgex_xmit(struct sk_buff *skb, struct net_device *dev);
static int kgex_xmit_nic_B(struct sk_buff *skb, struct net_device *dev);
static struct net_device_stats *kgex_get_stats(struct net_device *dev);
static struct net_device_stats *kgex_get_stats_nic_B(struct net_device *dev);
static int kgex_change_mtu(struct net_device *ndev, int new_mtu);
static int kgex_change_mtu_nic_B(struct net_device *ndev, int new_mtu);
static int kgex_set_mac_address(struct net_device *ndev, void *address);
static int kgex_set_mac_address_nic_B(struct net_device *ndev, void *address);

static const struct net_device_ops kgex_ops = {
	.ndo_open		= kgex_open,
	.ndo_stop		= kgex_stop,
	.ndo_start_xmit		= kgex_xmit,
	.ndo_get_stats		= kgex_get_stats,
	.ndo_set_mac_address    = kgex_set_mac_address,
	.ndo_change_mtu		= kgex_change_mtu,
};

static const struct net_device_ops kgex_ops_nic_B = {
	.ndo_open		= kgex_open_nic_B,
	.ndo_stop		= kgex_stop_nic_B,
	.ndo_start_xmit		= kgex_xmit_nic_B,
	.ndo_get_stats		= kgex_get_stats_nic_B,
	.ndo_set_mac_address    = kgex_set_mac_address_nic_B,
	.ndo_change_mtu		= kgex_change_mtu_nic_B,
};

static void kgex_rings_free(struct net_device *ndev);
static void kgex_dma_err_handler(unsigned long param);
static void kgex_tx_poll(unsigned long param);
static void kgex_rx_poll(unsigned long param);



//   Interrupt Handlers

static irqreturn_t kgex_tx_irq(int irq, void *_ndev);
static irqreturn_t kgex_rx_irq(int irq, void *_ndev);


static inline int kgex_check_net_device(uint32_t id)
{
    uint32_t i = 0;
    
    if (i>KGEX_MAX_NET_DEVS) return -1;    
    if (net_dev_tbl[i] == NULL) return 0;
    
    return 1;
}

static inline int kgex_check_tx_bd_space(struct net_private *lp, int num_frag)
{
	struct dma_bd *cur_p;
	cur_p = &lp->tx_bd_virt[(lp->tx_tail + num_frag) % TX_BD_NUM];
	if (cur_p->status & DMA_BD_STS_ALL_MASK)
		return NETDEV_TX_BUSY;
	return 0;
}

static int kgex_rings_init(struct net_device *ndev)
{
	int i = 0;
	int status = 0;
	uint32_t cr = 0;
	struct sk_buff *skb = NULL;
	struct net_private *lp = NULL;
        unsigned int timeout = 1000; //1 msec timeout	
        dma_addr_t temp = 0x0;
	
        volatile dma_addr_t tphys64 = 0x00000000EBCC2000;                
        volatile uint32_t tphys = 0;
        volatile uint32_t rg = 0;
        
        
	dbg1("kgex_rings_init()\n");
        
        if (ndev==NULL){prerr("ndev == NULL\n");return 1;}

	lp = netdev_priv(ndev);
        if (lp==NULL){prerr("lp == NULL\n");return 1;}

	lp->tx_curr = 0;
	lp->tx_tail = 0;
	lp->rx_curr = 0;
	
	status = dma_set_coherent_mask(&(ndev->dev), 0xFFFFFFFF);
	dbg1("kgex_rings_init(): dma_set_coherent_mask() status=%d\n", status);
        
        wmb();
       	dma_write32(lp, DMA_TX_CR_OFFSET, DMA_CR_RESET_MASK);
	while (dma_read32(lp, DMA_TX_CR_OFFSET) & DMA_CR_RESET_MASK)	
	{
		udelay(1);
		if (--timeout == 0)
		{
			prerr("ERROR: DMA TX reset timeout occured!\n");
			break;
		}
	}	

	dbg1("kgex_rings_init(): Reset DMA TX channel\n");
	
	//Allocate TX descriptors (DMA capable)
	lp->tx_bd_virt = dma_alloc_coherent(ndev->dev.parent, 
			sizeof(struct dma_bd) * TX_BD_NUM,
			&(lp->tx_bd_phys),
			GFP_KERNEL);

	if (lp->tx_bd_virt == NULL)
	{            
		prerr("ERROR: cannot allocate TX BD ring\n");
		return -ENOMEM;
	} 

	dbg1(KERN_INFO "kgex_rings_init(): lp->tx_bd_phys=0x%016llX\n", (uint64_t)lp->tx_bd_phys);
        dbg1(KERN_INFO "kgex_rings_init(): lp->tx_bd_virt=0x%016llX\n", (uint64_t)lp->tx_bd_virt);
	//Allocate RX descriptors (DMA capable)
	lp->rx_bd_virt = dma_alloc_coherent(ndev->dev.parent,
			sizeof(struct dma_bd) * RX_BD_NUM,
			&lp->rx_bd_phys,
			GFP_KERNEL);

	if (lp->rx_bd_virt == NULL)
	{
		prerr("ERROR: cannot allocate RX BD ring\n");
		return -ENOMEM;
	}
	dbg1(KERN_INFO "kgex_rings_init(): lp->rx_bd_phys=0x%016llX\n", (uint64_t)lp->rx_bd_phys);
        dbg1(KERN_INFO "kgex_rings_init(): lp->rx_bd_virt=0x%016llX\n", (uint64_t)lp->rx_bd_virt);

	memset(lp->tx_bd_virt, 0, sizeof(struct dma_bd) * TX_BD_NUM);
	memset(lp->rx_bd_virt, 0, sizeof(struct dma_bd) * RX_BD_NUM);
			
	/*Create linked-list of TX BDs*/
	for (i=0; i<TX_BD_NUM; i++)
	{
                dbg2("#%d lp->tx_bd_phys + sizeof(struct dma_bd) * ((i+1)modTX_BD_NUM) = 0x%016llX\n", i, (uint64_t)(lp->tx_bd_phys + sizeof(struct dma_bd) * ((i+1)%TX_BD_NUM)));
		lp->tx_bd_virt[i].next = LO32(lp->tx_bd_phys) + 
				sizeof(struct dma_bd) * ((i+1)%TX_BD_NUM);		                                
                dbg2("#%d lp->tx_bd_virt[%d].next = 0x%08X\n", i, i, lp->tx_bd_virt[i].next);			
	}

	/*Create linked-list and init RX BDs*/
	for (i=0; i<RX_BD_NUM; i++)
	{
		//Create physical linked-list
                if(HI32(lp->rx_bd_phys)!=0) {prwarn("kgex_rings_init(): HI32(lp->rx_bd_phys)");}
                
		lp->rx_bd_virt[i].next = LO32(lp->rx_bd_phys) + 
				sizeof(struct dma_bd) * ((i+1)%RX_BD_NUM);				
		
		////skb = dev_alloc_skb(lp->max_frm_size+2);		
		skb = netdev_alloc_skb_ip_align(ndev, lp->max_frm_size);		
		////skb = netdev_alloc_skb(ndev, lp->max_frm_size);
		if (!skb)
		{
			prerr("ERROR: cannot allocate RX skb No. #%u\n", i);
			goto out;			
		}		
		//DBG3 printk(KERN_INFO "kgex_rings_init(): skb=0x%X\n", (uint32_t)skb);
				
		//Give the pointer of the skb to the BD		
                lp->rx_bd_virt[i].rx_skb_virt_lo = LO32((uint64_t) skb);
		lp->rx_bd_virt[i].rx_skb_virt_hi = HI32((uint64_t) skb);
                
		temp = dma_map_single(ndev->dev.parent, 
                                      skb->data, 
                                      lp->max_frm_size,
                                    DMA_FROM_DEVICE);       
		if (dma_mapping_error(ndev->dev.parent, temp))
		{
			prerr("ERROR: cannot DMA map RX skb No. #%u\n", i);  
		}
                
                if(HI32(temp)!=0) {prwarn("kgex_rings_init(): HI32(temp)");}
                lp->rx_bd_virt[i].phys = LO32(temp); /*HI32 bits of temp are unusable*/
		lp->rx_bd_virt[i].cntrl = lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK |DMA_BD_CTRL_TXEOF_MASK;
	}
		
	cr = ReadReg32(lp->dma_regs, DMA_RX_CR_OFFSET);	

	/*Init RX part*/
//	cr = ReadReg32(lp->dma_regs, DMA_TX_CR_OFFSET);

#ifdef RX_ENABLE
        if(HI32(lp->rx_bd_phys)!=0) {prwarn("kgex_rings_init(): HI32(lp->rx_bd_phys)");}
	hw_write32(lp->dma_regs, DMA_RX_CDESC_OFFSET, LO32(lp->rx_bd_phys));	
	//hw_write(lp->dma_regs, DMA_RX_CR_OFFSET, 0x00001001);
        
        //RX IRQs coalescing
        cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	cr = ((cr & ~DMA_COALESCE_MASK) | ((lp->coalesce_count_rx) << DMA_COALESCE_SHIFT));        
        cr = (((cr & ~DMA_DELAY_MASK)) | (DMA_DFT_RX_WAITBOUND << DMA_DELAY_SHIFT));
        cr |= DMA_IRQ_ALL_MASK;
        
	hw_write32(lp->dma_regs, DMA_RX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
        dbg1("kgex_rings_init(): wrote to RX_CR value=0x%08X\n", (uint32_t)(DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK));
        dbg1("kgex_rings_init(): read RX_CR value=0x%08X\n", dma_read32(lp, DMA_RX_CR_OFFSET));
        dbg1("kgex_rings_init(): read RX_SR value=0x%08X\n", dma_read32(lp, DMA_RX_SR_OFFSET));
	hw_write32(lp->dma_regs, DMA_RX_TDESC_OFFSET, LO32(lp->rx_bd_phys) + 
					(sizeof(struct dma_bd) * (RX_BD_NUM -1)) );
#endif

	/*Init TX part*/
//	cr = ReadReg32(lp->dma_regs, DMA_TX_CR_OFFSET);
//	cr = (((cr & ~DMA_DELAY_MASK)) | (DMA_DFT_TX_WAITBOUND << DMA_DELAY_SHIFT));
//	cr |= DMA_IRQ_ALL_MASK;
	//hw_write32(lp->dma_regs, DMA_TX_CR_OFFSET, cr);	
#ifdef TX_ENABLE                			        
    
        //if(HI32(tx_phys)!=0) {prwarn("kgex_rings_init(): HI32(lp->tx_bd_phys)");}
        tphys64 = lp->tx_bd_phys;
        tphys = (uint32_t)LO32(tphys64);                
        
        //TODO: check why DMA reads different value comparted to the written one
        dbg1("kgex_rings_init(): tphys = 0x%016llX  LO32(tphys64) = 0x%08X lp->tx_bd_phys=0x%016llX\n",(uint64_t)tphys, LO32(tphys64), lp->tx_bd_phys);                   
	hw_write32(lp->dma_regs, DMA_TX_CDESC_OFFSET, tphys);   		
	//iowrite32(LO32(lp->tx_bd_phys), lp->dma_regs);

        dbg1("kgex_rings_init(): tphys = 0x%016llX  LO32(tphys64) = 0x%08X lp->tx_bd_phys=0x%016llX\n",(uint64_t)tphys, LO32(tphys64), lp->tx_bd_phys);        
        
        rg = ioread32((void __iomem *)((uint64_t)(lp->dma_regs) + DMA_TX_CDESC_OFFSET));
        dbg1("kgex_rings_init(): DMA_TX_CDESC_OFFSET = 0x%08X\n", rg);        
        rg = dma_read32(lp, DMA_TX_CDESC_OFFSET);
        dbg1("kgex_rings_init(): DMA_TX_CDESC_OFFSET = 0x%08X\n", rg);
    
        //TX IRQs coalescing
        cr = dma_read32(lp, DMA_TX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) | ((lp->coalesce_count_tx) << DMA_COALESCE_SHIFT));        
        cr = (((cr & ~DMA_DELAY_MASK)) | (DMA_DFT_TX_WAITBOUND << DMA_DELAY_SHIFT));
        cr |= DMA_IRQ_ALL_MASK;
        
        hw_write32(lp->dma_regs, DMA_TX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
        dbg1("kgex_rings_init(): wrote to TX_CR value=0x%08X\n", (uint32_t)(DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK));        
        
        dbg1("kgex_rings_init(): read TX_CR value=0x%08X\n", dma_read32(lp, DMA_TX_CR_OFFSET));
        dbg1("kgex_rings_init(): read TX_SR value=0x%08X\n", dma_read32(lp, DMA_TX_SR_OFFSET));
//	hw_write32(lp->dma_regs, DMA_TX_CR_OFFSET, 0x00001001);
#endif	
	
	dbg1("kgex_rings_init(): return 0;\n");
	return 0;
	
out:	
	//TODO: release bd rings
	printk(KERN_INFO "kgex_rings_init(): return -ENOMEM;\n");
	return -ENOMEM;
}

/**
 * kgex_xmit - Transmit an ethernet frame (called by the kernel)
 * @skb: socket buffer
 * @ndev: network device
 * return NETDEV_TX_OK on success, other if error or busy
 **/
static int kgex_xmit (struct sk_buff *skb, struct net_device *ndev)
{
	int i;
	struct net_private *lp;			
	uint32_t num_frag;
	struct dma_bd *cur_p;
	skb_frag_t *frag;
	dma_addr_t tail_phys;
        dma_addr_t temp = 0x0;	
        unsigned long flags;
				
	dbg1("%s: kgex_xmit(): ENTER\n", ndev->name);	
        
//         if(skb==NULL){prerr("kgex_xmit(): skb==NULL\n");}
//         
//         if(ndev==NULL){prerr("kgex_xmit(): ndev==NULL\n");}
//         if(&ndev->dev==NULL){prerr("kgex_xmit(): ndev->dev==NULL\n");}
//         
 	lp = netdev_priv(ndev);	
        dbg2("kgex_xmit(): read TX_SR value=0x%08X\n", dma_read32(lp, DMA_TX_SR_OFFSET));
        dbg2("kgex_xmit(): tail_phys=0x%08X\n", LO32(lp->tx_bd_phys));
        dbg2("kgex_xmit(): DMA_TX_CDESC_OFFSET=0x%08X\n", dma_read32(lp, DMA_TX_CDESC_OFFSET));
        
//         if(lp==NULL){prerr("kgex_xmit(): lp==NULL\n");}
//         
//         dbg1("kgex_xmit() skb=0x%016llX\n", (uint64_t)skb);
//         dbg1("kgex_xmit() ndev=0x%016llX\n", (uint64_t)ndev);
//         dbg1("kgex_xmit() lp=0x%016llX\n", (uint64_t)lp);
//         dbg1("kgex_xmit() skb->data=0x%016llX\n", (uint64_t)skb->data);
//         dbg1("kgex_xmit() ndev->dev.parent=0x%016llX\n", (uint64_t)&(ndev->dev.parent));
	
	//if ((skb->len<=0)){prerr("ERROR: got TX skb with len<=0\n");dev_kfree_skb_any(skb);return NETDEV_TX_OK;}
	
	num_frag = skb_shinfo(skb)->nr_frags;
//         dbg1("kgex_xmit() num_frag=%u\n", num_frag);
				
	//DEBUG
	//if (num_frag>0) {pr("TX frags=%u\n", num_frag);}
	
	if (kgex_check_tx_bd_space(lp, num_frag))
	{		
		pr("kgex_xmit(): NETDEV_TX_BUSY\n");
		if (!netif_queue_stopped(ndev)){netif_stop_queue(ndev);}		
		return NETDEV_TX_BUSY;		
	}		
	
	//DEBUG
	//if (spin_is_locked(&lp->tx_tail_lock)){	return NETDEV_TX_BUSY;}
#ifdef TX_THREADSAFE		
	//spin_lock_irqsave(&lp->tx_lock, flags);
	spin_lock_irq(&lp->tx_lock);
#endif        
	
   /*     if(&lp->tx_bd_virt[lp->tx_tail]==NULL){prerr("kgex_xmit(): &lp->tx_bd_virt[lp->tx_tail]==NULL\n");}
        else {dbg1("kgex_xmit() &lp->tx_bd_virt[lp->tx_tail]=0x%016llX\n", (uint64_t)(&lp->tx_bd_virt[lp->tx_tail]));}
   */     
	cur_p = &lp->tx_bd_virt[lp->tx_tail];
	cur_p->cntrl = skb_headlen(skb) | DMA_BD_CTRL_TXSOF_MASK;	
        
//         if(cur_p==NULL){prerr("kgex_xmit(): cur_p==NULL\n");}
//         else{dbg1("kgex_xmit() cur_p=0x%016llX\n", (uint64_t)cur_p);}
        
	temp = dma_map_single(ndev->dev.parent, skb->data, skb_headlen(skb), DMA_TO_DEVICE);        
//         dbg1("kgex_xmit() temp=0x%016llX\n", (uint64_t)temp);

        
        if(HI32(temp)!=0) {prwarn("kgex_xmit(): HI32(temp)");}        
        cur_p->phys = LO32(temp);                
        dbg2("kgex_xmit(): temp=0x%016llX cur_p->phys=0x%08X\n", (uint64_t)temp, cur_p->phys);
        
	for (i=0; i<num_frag; i++)
	{                
//                 if(lp==NULL){prerr("kgex_xmit(): i=%d, lp==NULL\n", i);}
//                 else{dbg1("kgex_xmit() i=%d, lp=0x%016llX\n", i, (uint64_t)lp);};
		++lp->tx_tail;                
		lp->tx_tail %= TX_BD_NUM;
                
//                 dbg1("kgex_xmit(): lp->tx_tail=%u\n", lp->tx_tail);
//                 if((&lp->tx_bd_virt[lp->tx_tail])==NULL){prerr("kgex_xmit(): &lp->tx_bd_virt[lp->tx_tail]==NULL\n");}
//                 else { dbg1("kgex_xmit() i=%d, &lp->tx_bd_virt[lp->tx_tail]=0x%016llX\n", i, (uint64_t)(&lp->tx_bd_virt[lp->tx_tail]));}
		
		cur_p = &lp->tx_bd_virt[lp->tx_tail];
                
//                 if(cur_p==NULL){prerr("kgex_xmit(): cur_p==NULL\n");}
//                 else{dbg1("kgex_xmit() cur_p=0x%016llX\n", (uint64_t)cur_p);}
		
                frag = &skb_shinfo(skb)->frags[i];
                
   /*             if (frag==NULL){prerr("kgex_xmit(): frag==NULL\n");}
                else {dbg1("kgex_xmit() frag=0x%016llX\n", (uint64_t)frag);}
   */             
//                 if (skb_frag_address(frag)==NULL){prerr("kgex_xmit(): skb_frag_address(frag)==NULL\n");}
//                 else{dbg1("kgex_xmit() i=%d, skb_frag_address(frag)=0x%016llX\n", i, (uint64_t)(skb_frag_address(frag)));}
                
		temp = dma_map_single(ndev->dev.parent,
					     skb_frag_address(frag),
					     skb_frag_size(frag),
					     DMA_TO_DEVICE);
                
		if(dma_mapping_error(ndev->dev.parent, temp))
		{
			prerr("kgex_xmit(): dma_mapping_error\n");
#ifdef TX_THREADSAFE		
                       // spin_unlock_irqrestore(&lp->tx_lock, flags);	
                        spin_unlock_irq(&lp->tx_lock);
#endif
		}
		cur_p->phys = temp;
		
		dbg3("kgex_xmit(): cur_p->cntrl = skb_frag_size(frag) = %u, cur_p->phys=0x%X\n", skb_frag_size(frag), (uint32_t)cur_p->phys);
		cur_p->cntrl = 0x0 | skb_frag_size(frag);
		//DEBUG
		//if (cur_p->cntrl > lp->max_frm_size){pr("TX packet length = %u\n", cur_p->cntrl)};		
	}
	
// 	dbg1("kgex_xmit(): oustide for\n");
	
	cur_p->cntrl |= DMA_BD_CTRL_TXEOF_MASK;
        
        dbg2("kgex_xmit(): skb=0x%016llX\n", (uint64_t)skb);
	cur_p->tx_skb_virt_lo = LO32((uint64_t)skb);
	cur_p->tx_skb_virt_hi = HI32((uint64_t)skb);
        dbg2("kgex_xmit(): cur_p->tx_skb_virt_lo=0x%08X cur_p->tx_skb_virt_hi=0x%08X\n", cur_p->tx_skb_virt_lo, cur_p->tx_skb_virt_hi);
        


	/*Find last BD physical address*/
	tail_phys = lp->tx_bd_phys + sizeof(struct dma_bd) * lp->tx_tail;
        dbg2("kgex_xmit(): tail_phys=0x%016llX\n", (uint64_t)tail_phys);
	wmb();
	
//         dbg1("kgex_xmit(): tail_phys=0x%016llX\n", (uint64_t)tail_phys);
//         if (lp->dma_regs==NULL) {prerr("kgex_xmit(): lp->dma_regs==NULL\n");}
//         else{dbg1("kgex_xmit(): lp->dma_regs=0x%016llX\n", (uint64_t)(lp->dma_regs));}
        
        
	/*Start DMA transfer*/	
        if(HI32(tail_phys)!=0) {prwarn("kgex_xmit(): HI32(tail_phys)");}
	dma_write32(lp, DMA_TX_TDESC_OFFSET, LO32(tail_phys));		
        dbg2("kgex_xmit(): DMA_TX_TDESC_OFFSET=0x%08X\n", dma_read32(lp, DMA_TX_TDESC_OFFSET));
        
//         dbg1("kgex_xmit(): wrote DMA_TX_TDESC_OFFSET\n");
	
	/*Compute next new tail*/
	++(lp->tx_tail);
	lp->tx_tail %= TX_BD_NUM;
        
        if (tx_poll!=0)
        {
            dbg2("kgex_xmit(): scheduling debug tasklet kgex_tx_poll.\n");
            tasklet_hi_schedule(&lp->tx_poll_task);
        }
	
	dbg1("%s: kgex_xmit(): RETURN\n", ndev->name);	
#ifdef TX_THREADSAFE        
	//spin_unlock_irqrestore(&lp->tx_lock, flags);
        spin_unlock_irq(&lp->tx_lock);
#endif        
	return NETDEV_TX_OK;        
}

/**
 * kgex_xmit_done - Called by the interrupt handler. Updares descriptors
 * and indexes.
 * @ndev: network device 
 **/
static void kgex_xmit_done(struct net_device *ndev)
{
	unsigned int status = 0;	
	uint32_t size = 0;
	uint32_t packets = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd;	
        uint64_t temp = 0x0;

	dbg1("%s: kgex_xmit_done() ENTER\n", ndev->name);	

	lp = netdev_priv(ndev);		
	
	cur_bd = &lp->tx_bd_virt[lp->tx_curr];
	status = cur_bd->status;
	while ((status & DMA_BD_STS_COMPLETE_MASK) || (status & DMA_BD_STS_ALL_ERR_MASK)) 		
	{	
                dbg2("kgex_xmit_done(): cur_bd->phys = 0x%08X\n", cur_bd->phys);
                dbg2("kgex_xmit_done(): cur_bd->next = 0x%08X\n", cur_bd->next);
		if (status & DMA_BD_STS_DEC_ERR_MASK){prerr("ERROR: DMA RX descriptor DMA_BD_STS_DEC_ERR_MASK\n");}						
		if (status & DMA_BD_STS_INT_ERR_MASK){prerr("ERROR: DMA RX descriptor DMA_BD_STS_INT_ERR_MASK\n");}
		if (status & DMA_BD_STS_SLV_ERR_MASK){prerr("ERROR: DMA RX descriptor DMA_BD_STS_SLV_ERR_MASK\n");}				
		
		dma_unmap_single(ndev->dev.parent, (dma_addr_t)cur_bd->phys,
				(cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK),
				DMA_TO_DEVICE);		
		if (cur_bd->tx_skb_virt_lo || cur_bd->tx_skb_virt_hi)
		{
			dbg3("kgex_xmit_done(): cur_bd->app4!=NULL\n");
                        temp = WHOLE(cur_bd->tx_skb_virt_lo, cur_bd->tx_skb_virt_hi);
			dev_kfree_skb_irq((struct sk_buff *)temp);
		}
		/*cur_p->phys = 0;*/
		cur_bd->app0 = 0;
		cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;						
		cur_bd->status = 0;

		size += status & DMA_BD_STS_ACTUAL_LEN_MASK;
		packets++;
		
		++lp->tx_curr;
		lp->tx_curr %= TX_BD_NUM;
		
		cur_bd = &lp->tx_bd_virt[lp->tx_curr];
		status = cur_bd->status;
	}

	ndev->stats.tx_packets += packets;
	ndev->stats.tx_bytes += size;	
	netif_wake_queue(ndev);	
        dbg1("%s: kgex_xmit_done() RETURN\n", ndev->name);	
}

int kgex_recv(struct net_device *ndev, int budget)
{		
		unsigned long packets = 0;
		unsigned long len = 0;
		unsigned long size = 0;
                unsigned long dropped = 0;
                unsigned long rx_err = 0;
		dma_addr_t tail_phys = 0;
		struct net_private *lp = NULL;
		struct sk_buff *skb, *new_skb;
		struct dma_bd *cur_bd;
		int netrx = 0;
		unsigned int i = 0;		
                uint64_t temp = 0;
                dma_addr_t phys_cpu = 0x0;
                
                uint32_t freebds = 0;
		
//		dbg1("kgex_recv(): ENTER\n");		
		lp = netdev_priv(ndev);
		
		rmb();
		cur_bd = &lp->rx_bd_virt[lp->rx_curr];		
						
		while((freebds<budget) && (cur_bd->status & DMA_BD_STS_COMPLETE_MASK) || (cur_bd->status & DMA_BD_STS_ALL_ERR_MASK))
		{			
			i = i + 1; 			
			tail_phys = lp->rx_bd_phys + sizeof(struct dma_bd) * lp->rx_curr;
			rmb();					
			
			if ((cur_bd->status & DMA_BD_STS_DEC_ERR_MASK)){prerr("DMA RX descriptor DMA_BD_STS_DEC_ERR_MASK\n");}			
			if ((cur_bd->status & DMA_BD_STS_INT_ERR_MASK)){prerr("DMA RX descriptor DMA_BD_STS_INT_ERR_MASK\n");}
			if ((cur_bd->status & DMA_BD_STS_SLV_ERR_MASK)){prerr("DMA RX descriptor DMA_BD_STS_SLV_ERR_MASK\n");}				
			
			temp = WHOLE(cur_bd->rx_skb_virt_lo, cur_bd->rx_skb_virt_hi);	
			skb = (struct sk_buff *)(temp);
			len = cur_bd->status & DMA_BD_STS_ACTUAL_LEN_MASK;
			dbg2("kgex_recv(): len=%u\n", len);	
                        
			if (len>lp->max_frm_size){pr("RX packet length = %u\n", len);}
							
			//dma_unmap_single(ndev->dev.parent, cur_bd->phys, lp->max_frm_size, DMA_FROM_DEVICE);
			skb->dev = ndev;
			skb_put(skb, len);
			skb->protocol = eth_type_trans(skb, ndev);			
			skb->ip_summed = CHECKSUM_UNNECESSARY;								
			
			//netrx = netif_rx(skb);
			netrx = netif_receive_skb(skb);			
                        
			if (netrx == NET_RX_SUCCESS)
			{
                            dbg1("kgex_recv(): NET_RX_SUCCESS, #%u, status=0x%X\n", i, cur_bd->status);
                            packets ++;
                            size += len;
			}else
			{
                            prwarn("kgex_recv(): NET_RX_DROP, #%u, status=0x%X\n", i, cur_bd->status);						
                            dropped++;                            
			}						
			
			
			////new_skb = dev_alloc_skb(lp->max_frm_size+2);
			new_skb = netdev_alloc_skb_ip_align(ndev, lp->max_frm_size);
			////new_skb = netdev_alloc_skb(ndev, lp->max_frm_size);
			if (new_skb==NULL)
			{
				prerr("ERROR: cannot allocate new RX skb\n");				
				return 1;
			}
			
			wmb();			                        
			phys_cpu = dma_map_single(ndev->dev.parent, 
										  new_skb->data, 
										  lp->max_frm_size,
										  DMA_FROM_DEVICE);
                        
                        if(HI32(phys_cpu)!=0) {prwarn("kgex_recv(): HI32(phys_cpu)");}
                        cur_bd->phys = LO32(phys_cpu);
			cur_bd->cntrl = lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK |DMA_BD_CTRL_TXEOF_MASK;
			cur_bd->status = 0x0;			
                        cur_bd->rx_skb_virt_lo = LO32((uint64_t)new_skb);
			cur_bd->rx_skb_virt_hi = HI32((uint64_t)new_skb);
			
			++lp->rx_curr;
			lp->rx_curr %= RX_BD_NUM;
                        
                        rmb();
			cur_bd = &lp->rx_bd_virt[lp->rx_curr];
                        
                        freebds += 1;
		}
		
		ndev->stats.rx_packets += packets;
		ndev->stats.rx_bytes += size;
                ndev->stats.rx_dropped += dropped;

		if (tail_phys)
		{
                    dma_write32(lp, DMA_RX_TDESC_OFFSET, LO32(tail_phys));			
		}	
				
		return freebds;
}

static void kgex_dma_err_handler(unsigned long param)
{
	unsigned int i = 0;
	unsigned int timeout = 1000; //1 msec timeout	
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd = NULL;        
        uint64_t temp = 0;
        
        struct net_device *ndev = NULL;
        ndev = (struct net_device *)param;
	
	lp = netdev_priv(ndev);
	
	pr("DMA error handler tasklet\n");
	/*Reset DMA TX path*/
	dma_write32(lp, DMA_TX_CR_OFFSET, DMA_CR_RESET_MASK);
	while (dma_read32(lp, DMA_TX_CR_OFFSET) & DMA_CR_RESET_MASK)	
	{
		udelay(1);
		if (--timeout == 0)
		{
			prerr("ERROR: DMA TX reset timeout occured!\n");
			break;
		}
	}
	
	/*Reset DMA RX path*/
	dma_write32(lp, DMA_RX_CR_OFFSET, DMA_CR_RESET_MASK);
	while (dma_read32(lp, DMA_RX_CR_OFFSET) & DMA_CR_RESET_MASK)	
	{
		udelay(1);
		if (--timeout == 0)
		{
			prerr("ERROR: DMA RX reset timeout occured!\n");
			break;
		}
	}
	
	/*Reset TX BD ring*/
	for(i=0; i<TX_BD_NUM; i++)
	{
		cur_bd = &lp->tx_bd_virt[i];
		if (cur_bd->phys)
		{
			dma_unmap_single(ndev->dev.parent, (dma_addr_t)cur_bd->phys, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_TO_DEVICE);			
		}
		if (cur_bd->tx_skb_virt_lo || cur_bd->tx_skb_virt_hi)
		{
                        temp = WHOLE(cur_bd->tx_skb_virt_lo, cur_bd->tx_skb_virt_hi);
			dev_kfree_skb_irq((struct sk_buff *)temp);			
		}
		cur_bd->phys = 0;
		cur_bd->cntrl = 0;
		cur_bd->status = 0;		
		cur_bd->app0 = 0;
		cur_bd->rx_skb_virt_lo = 0;
		cur_bd->rx_skb_virt_hi = 0;	
                cur_bd->app3 = 0;		
                cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
	}
	
	/*Reset RX BD ring*/
	for(i=0; i<RX_BD_NUM; i++)
	{
		cur_bd = &lp->rx_bd_virt[i];
		cur_bd->status = 0;
		cur_bd->app0 = 0;
		cur_bd->rx_skb_virt_lo = 0;
		cur_bd->rx_skb_virt_hi = 0;			
		cur_bd->app3 = 0;
		cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
	}
	
	lp->tx_curr = 0;
	lp->tx_tail = 0;
	lp->rx_tail = 0;

#ifdef RX_ENABLE
	hw_write32(lp->dma_regs, DMA_RX_CDESC_OFFSET, LO32(lp->rx_bd_phys));	
	//hw_write32(lp->dma_regs, DMA_RX_CR_OFFSET, 0x00001001);
	hw_write32(lp->dma_regs, DMA_RX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
	hw_write32(lp->dma_regs, DMA_RX_TDESC_OFFSET, LO32(lp->rx_bd_phys) + 
					(sizeof(struct dma_bd) * (RX_BD_NUM -1)) );
#endif	
	
#ifdef TX_ENABLE
	hw_write32(lp->dma_regs, DMA_TX_CDESC_OFFSET, LO32(lp->tx_bd_phys));   			
	hw_write32(lp->dma_regs, DMA_TX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
//	hw_write32(lp->dma_regs, DMA_TX_CR_OFFSET, 0x00001001);
#endif				
}

static void kgex_rings_free(struct net_device *ndev)
{
        uint32_t i = 0;
        struct net_private *lp = NULL;        
        struct dma_bd *cur_bd;
        uint64_t temp = 0;
        struct sk_buff *skb = NULL;
        
        
        dbg1("kgex_rings_free(): ENTER\n");		
	lp = netdev_priv(ndev);
                
        dbg1("kgex_rings_free(): unmapping TX BDs and freeing skbs\n");
	/*Free TX BD ring*/
	for(i=0; i<TX_BD_NUM; i++)
	{
		cur_bd = &lp->tx_bd_virt[i];
          //      dbg1("kgex_rings_free() #%u lp->tx_bd_phys[%u]=0x%016llX\n", i, i,(uint64_t)(lp->tx_bd_phys + sizeof(struct dma_bd) * ((i+1)%TX_BD_NUM)));
            //    dbg1("kgex_rings_free() #%u cur_bd->phys=0x%08X\n",i, cur_bd->phys);
              //  temp = WHOLE(cur_bd->tx_skb_virt_lo, cur_bd->tx_skb_virt_hi)
               // dbg1("kgex_rings_free() #%u cur_bd->tx_virt=0x%016llX\n",i, temp);
                
		if (cur_bd->phys)
		{
			dma_unmap_single(ndev->dev.parent, (dma_addr_t)cur_bd->phys, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_TO_DEVICE);		
		}
// 		if (cur_bd->tx_skb_virt_lo || cur_bd->tx_skb_virt_hi)
// 		{
//                         temp = WHOLE(cur_bd->tx_skb_virt_lo, cur_bd->tx_skb_virt_hi);
// 			dev_kfree_skb_irq((struct sk_buff *)temp);			
// 		}
		cur_bd->phys = 0;
		cur_bd->cntrl = 0;
		cur_bd->status = 0;		
		cur_bd->app0 = 0;
		cur_bd->rx_skb_virt_lo = 0;
		cur_bd->rx_skb_virt_hi = 0;	
                cur_bd->app3 = 0;		
                cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
	}
	dbg1("kgex_rings_free(): freeing TX BD ring\n");
	dma_free_coherent(ndev->dev.parent, sizeof(struct dma_bd) * TX_BD_NUM, lp->tx_bd_virt, lp->tx_bd_phys);        
        
        dbg1("kgex_rings_free(): unmapping RX BDs and freeing skbs\n");
        /*Free RX BD ring*/
  	for(i=0; i<RX_BD_NUM; i++)
	{
		cur_bd = &lp->rx_bd_virt[i];   
                //dbg1("kgex_rings_free() #%u lp->rx_bd_phys[%u]=0x%016llX\n", i, i,(uint64_t)(lp->rx_bd_phys + sizeof(struct dma_bd) * ((i+1)%RX_BD_NUM)));
                //dbg1("kgex_rings_free() #%u cur_bd->phys=0x%08X\n",i, cur_bd->phys);
                //temp = WHOLE(cur_bd->rx_skb_virt_lo, cur_bd->rx_skb_virt_hi)
                //dbg1("kgex_rings_free() #%u cur_bd->rx_virt=0x%016llX\n",i, temp);

                
                if (cur_bd->rx_skb_virt_lo || cur_bd->rx_skb_virt_hi)
                {
                        temp = WHOLE(cur_bd->rx_skb_virt_lo, cur_bd->rx_skb_virt_hi);
                        skb = (struct sk_buff *)temp;             
                        dma_unmap_single(ndev->dev.parent, (dma_addr_t)cur_bd->phys, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_FROM_DEVICE);                       
			dev_kfree_skb_irq(skb);                    
                }                                
                
		cur_bd->status = 0;
		cur_bd->app0 = 0;
		cur_bd->rx_skb_virt_lo = 0;
		cur_bd->rx_skb_virt_hi = 0;			
		cur_bd->app3 = 0;
		cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
	}     
	dbg1("kgex_rings_free(): freeing RX BD ring\n");
	dma_free_coherent(ndev->dev.parent, sizeof(struct dma_bd) * RX_BD_NUM, lp->rx_bd_virt, lp->rx_bd_phys);
        	
	dbg1("kgex_rings_free(): RETURN\n");	
}

/*========== Interrupt Handlers ==================*/
static irqreturn_t kgex_tx_irq(int irq, void *_ndev)
{
	unsigned int status = 0;	
	struct net_device *ndev;
	struct net_private *lp;
#ifdef TX_THREADSAFE        
	unsigned long flags;
#endif        
	uint32_t cr = 0;
//	spin_lock(&lp->lock);
	
	
	
	ndev = _ndev;
	lp = netdev_priv(ndev);	
        
        dbg1("%s: kgex_tx_irq(): ENTER\n", ndev->name);

#ifdef TX_THREADSAFE        
	spin_lock_irqsave(&lp->tx_lock, flags);
#endif        
	
//	status = ReadReg32(lp->dma_regs, 0x4);
	status = dma_read32(lp, DMA_TX_SR_OFFSET);
	dbg2("kgex_tx_irq(): status = 0x%X\n", status);		
	//dma_write32(lp, DMA_TX_SR_OFFSET, status);						
	
	if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK))
	{
		dma_write32(lp, DMA_TX_SR_OFFSET, status);	
		kgex_xmit_done(lp->ndev);		
#ifdef TX_THREADSAFE                
		spin_unlock_irqrestore(&lp->tx_lock, flags);
#endif                
		return IRQ_HANDLED;
	}
	
	if (status & DMA_IRQ_ERROR_MASK) 
	{
		prerr( "ERROR: TX DMA error, status=0x%X\n", status);				
		
		/*Disable error interrupts for TX*/
		cr = dma_read32(lp, DMA_TX_CR_OFFSET);
		cr &= (~DMA_IRQ_ALL_MASK);		
		dma_write32(lp, DMA_TX_CR_OFFSET, cr);
		
		/*Disable error interrupts for RX*/
		cr = dma_read32(lp, DMA_RX_CR_OFFSET);
		cr &= (~DMA_IRQ_ALL_MASK);
		dma_write32(lp, DMA_RX_CR_OFFSET, cr);
		
		tasklet_schedule(&lp->dma_err_task);
		dma_write32(lp, DMA_TX_SR_OFFSET, status);
                                                                
	}				

	dbg1("%s: kgex_tx_irq(): RETURN\n", ndev->name);
#ifdef TX_THREADSAFE        
	spin_unlock_irqrestore(&lp->tx_lock, flags);
#endif        
	return IRQ_HANDLED;
}

static irqreturn_t kgex_rx_irq(int irq, void *_ndev)
{
	struct net_device *ndev;
	struct net_private *lp;	
        uint32_t cr = 0;
	unsigned int status = 0;	
#ifdef RX_THREADSAFE        
	unsigned long flags;
#endif        
		
	ndev = _ndev;
	lp = netdev_priv(ndev);	

	dbg1("%s: kgex_rx_irq(): ENTER\n", ndev->name);        
        
#ifdef RX_THREADSAFE	
	//spin_lock_irqsave(&lp->rx_lock, flags);
#endif					
	//status = ReadReg(lp->dma_regs, DMA_RX_SR_OFFSET);
	status = dma_read32(lp, DMA_RX_SR_OFFSET);
	if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK)) 
	{	
                cr = dma_read32(lp, DMA_RX_CR_OFFSET);
                cr &= ~(DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK);
                dma_write32(lp, DMA_RX_CR_OFFSET, cr);
		//dma_write32(lp, DMA_RX_SR_OFFSET, status);			
		//kgex_recv(ndev);				
                napi_schedule(&lp->napi);
		goto out_normal;
	}	
	
	if (!(status & DMA_IRQ_ALL_MASK))
        {
            prerr("No interrupts for %s RX.\n", ndev->name);
        }
        
	if (status & DMA_IRQ_ERROR_MASK)
	{
		dma_write32(lp, DMA_RX_SR_OFFSET, status);
                ndev->stats.rx_errors++;
		prerr("ERROR: RX DMA error, status=0x%X\n", status);
		//kgex_reset(ndev);
	}

out_normal:
	dbg1("%s: kgex_rx_irq(): RETURN\n", ndev->name);
#ifdef RX_THREADSAFE        
	//spin_unlock_irqrestore(&lp->rx_lock, flags);		
#endif        
	return IRQ_HANDLED;
}
/*========== End of  Interrupt Handlers ==========*/

static void kgex_tx_poll(unsigned long ethid)
{
    struct net_device *ndev = NULL;
    struct net_private *lp;
    uint32_t status = 0;
    uint32_t cr = 0;
    
    dbg1("kgex_tx_poll() ENTER\n");
    
    ndev = net_dev_tbl[ethid];    
    lp = netdev_priv(ndev);

    status = dma_read32(lp, DMA_TX_CDESC_OFFSET);
    dbg2("kgex_tx_poll(): DMA_TX_CDESC_OFFSET = 0x%08X\n", status);	
    
    status = dma_read32(lp, DMA_TX_TDESC_OFFSET);
    dbg2("kgex_tx_poll(): DMA_TX_TDESC_OFFSET = 0x%08X\n", status);	


    status = dma_read32(lp, DMA_TX_CR_OFFSET);
    dbg1("kgex_tx_poll(): DMA_TX_CR_OFFSET = 0x%08X\n", status);	        
    status = dma_read32(lp, DMA_TX_SR_OFFSET);
    dbg1("kgex_tx_poll(): DMA_TX_SR_OFFSET = 0x%08X\n", status);	
    
    
    
    if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK))     
    {
	dma_write32(lp, DMA_TX_SR_OFFSET, status);	        
        kgex_xmit_done(lp->ndev);        
    }
    if (status & DMA_IRQ_ERROR_MASK)
    {
        /*Disable error interrupts for TX*/
        prerr("ERROR: TX DMA error, status = 0x%08X\n", status);        
        cr = dma_read32(lp, DMA_TX_CR_OFFSET);
        cr &= (~DMA_IRQ_ALL_MASK);		
	dma_write32(lp, DMA_TX_CR_OFFSET, cr);
        
        kgex_xmit_done(ndev);
        
        /*Disable error interrupts for RX*/
        cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	cr &= (~DMA_IRQ_ALL_MASK);
	dma_write32(lp, DMA_RX_CR_OFFSET, cr);      
                
        dma_write32(lp, DMA_TX_SR_OFFSET, status);
    }            
}

static int kgex_napi_rx_poll(struct napi_struct *napi, int quota)
{
    struct net_private *lp = NULL;
    int done = 0;
    uint32_t status = 0;
    uint32_t cr = 0;
            
    lp = container_of(napi, struct net_private, napi);
    
    spin_lock(&lp->rx_lock);
    status = dma_read32(lp, DMA_RX_SR_OFFSET);
    while((status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK)) && (done < quota)) 
    {
        dma_write32(lp, DMA_RX_SR_OFFSET, status);
        if (status & DMA_IRQ_ERROR_MASK) {prerr("%s: RX Error\n", lp->ndev->name); break;}
        
        done += kgex_recv(lp->ndev, quota - done);
        status = dma_read32(lp, DMA_RX_SR_OFFSET);
    }
    spin_unlock(&lp->rx_lock);
    
    if(done < quota)
    {
        napi_complete(napi);
        cr = dma_read32(lp, DMA_RX_CR_OFFSET);
        cr |= (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK);
        dma_write32(lp, DMA_RX_CR_OFFSET, cr);
    }        
    
    return done;
}

static void kgex_rx_poll(unsigned long ethid)
{
    struct net_device *ndev = NULL;
    struct net_private *lp = NULL;
    uint32_t status = 0;
    uint32_t cr = 0;
#ifdef RX_THREADSAFE        
	unsigned long flags;
#endif     
    
    ndev = net_dev_tbl[ethid];    
    lp = netdev_priv(ndev);
    
#ifdef RX_THREADSAFE	
	spin_lock_irqsave(&lp->rx_lock, flags);
#endif		    
    status = dma_read32(lp, DMA_RX_SR_OFFSET);
    dbg1("kgex_rx_poll(): DMA_RX_SR_OFFSET = 0x%08X\n", status);	
    if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK)) 
    {	
	dma_write32(lp, DMA_RX_SR_OFFSET, status);			
	kgex_recv(lp->ndev, 0);				
	goto out_normal;
    }   
    
    if (status & DMA_IRQ_ERROR_MASK)
    {
	dma_write32(lp, DMA_RX_SR_OFFSET, status);	
	prerr("ERROR: RX DMA error, status=0x%X\n", status);	
        
        /*Disable error interrupts for RX*/
        cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	cr &= (~DMA_IRQ_ALL_MASK);
	dma_write32(lp, DMA_RX_CR_OFFSET, cr);                      
        dma_write32(lp, DMA_TX_SR_OFFSET, status);        
    }   
    
out_normal:    
#ifdef RX_THREADSAFE        
    spin_unlock_irqrestore(&lp->rx_lock, flags);		
#endif         
    
    tasklet_schedule(&lp->rx_poll_task);    
    
 //   dbg1("kgex_rx_poll() RETURN\n");        
}

/*========== Network Device Functions ====*/
int kgex_open(struct net_device *ndev)
{
	int status = -1;
	struct net_private *lp;	

	dbg1("kgex_open(): ENTER\n");

	lp = netdev_priv(ndev);	
	lp->cnt = 0;
	
        if (lp->jumbo_support==1)
        {
            lp->max_frm_size = KGE_MAX_JUMBO_FRAME_SIZE;
            ndev->mtu = KGE_JUMBO_MTU;
        }else
        {
            //lp->max_frm_size = KGE_MAX_VLAN_FRAME_SIZE;    
            lp->max_frm_size = KGE_MAX_FRM_SIZE;	
        }
                
                        			
	pr("set max Ethernet Frame size to %u Bytes, ndev->mtu = %u Bytes\n", lp->max_frm_size, ndev->mtu);
	
	status = kgex_rings_init(ndev);
	if (status!=0){return status;}
	
	napi_enable(&lp->napi);

	dbg1("kgex_open(): TX irq = %d, RX irq = %d\n", lp->tx_irq, lp->rx_irq);	                
        
        /* Help: IRQ flags
         * IRQF_SHARED = 0x80
         */
        
        if (tx_poll==0)
        {
            //Request TX irq
            status = request_irq(lp->tx_irq, kgex_tx_irq, IRQF_SHARED, ndev->name, ndev);
            if (status){prerr("ERROR: cannot get TX IRQ\n");return -ENOMEM;}
            pr("Got TX IRQ %d\n", lp->tx_irq);
        }

        if (rx_poll==0)
        {
            //Request RX irq
            status = request_irq(lp->rx_irq, kgex_rx_irq, IRQF_SHARED, ndev->name, ndev);
            if (status){prerr("ERROR: cannot get RX IRQ\n");return -ENOMEM;}
            pr("Got RX IRQ %d\n", lp->rx_irq);
        }
	
//	irq_set_irq_type(lp->tx_irq, IRQ_TYPE_EDGE_RISING);
//	irq_set_irq_type(lp->rx_irq, IRQ_TYPE_EDGE_RISING);

	/*Create proc entry*/
#ifdef ENABLE_PROC        
	create_proc_eth10g(ndev);
#endif                
        
	netif_start_queue(ndev);

        if (rx_poll!=0)
        {
            dbg1("kgex_open(): enabling rx_poll_task\n");
            tasklet_schedule(&lp->rx_poll_task);
        }
	
	pr("opened successfully!\n");
	dbg1("kgex_open(): RETURN\n");
	return 0;
}

int kgex_stop(struct net_device *ndev)
{
	struct net_private *lp = NULL;
        uint32_t cr = 0;
        
        dbg1("kgex_stop() ENTER\n");
	
	lp = netdev_priv(ndev);
        
	cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	dma_write32(lp, DMA_RX_CR_OFFSET, cr & (~DMA_CR_RUNSTOP_MASK));        
        cr = dma_read32(lp, DMA_TX_CR_OFFSET);
	dma_write32(lp, DMA_TX_CR_OFFSET, cr & (~DMA_CR_RUNSTOP_MASK));        
        
        napi_disable(&lp->napi);
        		
	//netif_stop_queue(ndev);       

#ifdef ENABLE_PROC                
        /*Remove proc entry*/
	remove_proc_eth10g(ndev);
#endif                
        
//         tasklet_disable(&(lp->dma_err_task));
//         tasklet_disable(&(lp->kgex_tx_poll));
        
        kgex_rings_free(ndev);
        
        free_irq(lp->tx_irq, ndev);
        free_irq(lp->rx_irq, ndev);		
	
        pr("interface down\n");
        dbg1("kgex_stop() RETURN\n");
        
	return 0;
}

static struct net_device_stats *kgex_get_stats(struct net_device *ndev)
{	
	return &ndev->stats;	
}

static int kgex_change_mtu(struct net_device *ndev, int new_mtu)
{
	struct net_private *lp = NULL;
        
	lp = netdev_priv(ndev);

	if (netif_running(ndev)) return -EBUSY;                        
        if ((new_mtu > KGE_JUMBO_MTU) || (new_mtu < 64)) return -EINVAL;
        if (new_mtu + VLAN_ETH_HLEN + KGE_TRL_SIZE > lp->max_frm_size) return -EINVAL;        
                
        ndev->mtu = new_mtu;
        
	return 0;
}

static int kgex_set_mac_address(struct net_device *ndev, void *address)
{
//	struct net_private *lp = netdev_priv(ndev);
	struct sockaddr *addr = address;
	void *mac;

//JVELEG: DEBUG
	return 0;

	mac = addr->sa_data;
	if (mac)
	{
		if (!is_valid_ether_addr(mac))
		{
		printk(KERN_INFO "invalid mac\n");
			eth_random_addr(ndev->dev_addr);
		}else
		{
			printk(KERN_INFO "copying mac\n");
			memcpy(ndev->dev_addr, mac, ndev->addr_len);		
		}
	}
//		memcpy(ndev->dev_addr, mac, ETH_ALEN);

	return 0;
}


/*========== End of Network Device Functions ====*/

/*===== Platform Driver Functions ===============*/
int kgex_init (struct net_device **dev_out)
{
	pr("kgex_init()\n");	
	return 0;
}

static int kgex_probe(struct platform_device *op)
{
	int status;
	//int size;
	struct device_node *np;
	struct net_device *ndev;
	struct net_private *lp;
        
        struct net_private *temp;
        
        void *prop = NULL;
        
	//char *addr;	
        //struct resource dma_res;
        phys_addr_t mac_phys_addr;
	phys_addr_t mac_length;        
	phys_addr_t dma_phys_addr;
	phys_addr_t dma_length;        
        
        
	dbg1("kgex_probe(): ENTER\n");	

        mac_phys_addr = 0xB0093000;
        mac_length = 0x1000;
        

        dma_phys_addr = BASE_NIC_A_DMA; /*For Network FPGA NIC*/        
//        dma_phys_addr = BASE_NIC_C_DMA; /*For Storage FPGA NIC*/        
        dma_length = LEN_DMA_REGS;
        

		

	ndev = alloc_etherdev(sizeof(struct net_private));
	if (!ndev)
	{
		prerr("ERROR: Could not allocate ethernet device\n");
		return -1;
	}
	//dev_set_drvdata(&op->dev, ndev);
	platform_set_drvdata(op, ndev);
	SET_NETDEV_DEV(ndev, &op->dev);
	//ether_setup(ndev);

	ndev->netdev_ops = &kgex_ops;
	//ndev->ethtool_ops = &kgex_ethtool_ops;
#ifdef ENABLE_ETHTOOL        
	eth10g_ethtool_init(ndev);
#endif        
//	ndev->hard_header_len = 14;
	ndev->addr_len = ETH_ALEN; //added recently (usable?)
	//ndev->flags &= ~IFF_MULTICAST;  /* clear multicast */
	//ndev->flags |= IFF_NOARP;
	ndev->flags |= IFF_BROADCAST;
        ndev->flags |= IFF_MULTICAST;
	
////	ndev->features = NETIF_F_SG | NETIF_F_FRAGLIST; 	
////	ndev->features |= NETIF_F_IP_CSUM | NETIF_F_HW_CSUM;	
	//ndev->features = NETIF_F_IP_CSUM |  NETIF_F_SG;
	ndev->features = NETIF_F_SG;
//	ndev->features |= NETIF_F_RXCSUM;


	strcpy(ndev->name, "eth%d");

	lp = netdev_priv(ndev);
	lp->ndev = ndev;
	lp->dev = &op->dev;
        
  

	status = register_netdev(ndev);
	if (status) {
		prerr("ERROR: cannot register network device, errno=%d\n", status);
		return status;
	}
	
        prop = of_get_property(op->dev.of_node, "eth-id", NULL);
        if (prop==NULL){prerr("cannot get eth interface ID from DTB.\n");return 1;}
        
        lp->id = be32_to_cpup(prop);
        pr("Eth ID = %d\n", lp->id);
                
        if (lp->id == ETH_ID_NIC_A)
        {
            dma_phys_addr = BASE_NIC_A_DMA;
            dma_length = LEN_DMA_REGS;
            
            lp->dma_regs = ioremap(dma_phys_addr, dma_length);        
            if (lp->dma_regs == NULL)
            {
                    dev_err(&op->dev, "ERROR: cannot map DMA device\n");
                    return -ENODEV;
            }
            dbg1("kgex_probe(): ioremapped lp->dma_regs phys=0x%08llX virt=0x%016llX\n", dma_phys_addr, (uint64_t)lp->dma_regs);	            

            if (ex_nr==0)
            {
                memcpy(lp->ndev->dev_addr, exynos_0_mac_A_addr, ndev->addr_len);                                
            }
            else if (ex_nr==1)
            {
                memcpy(lp->ndev->dev_addr, exynos_1_mac_A_addr, ndev->addr_len);                                
            }else if(ex_nr==2)
            {
                memcpy(lp->ndev->dev_addr, exynos_2_mac_A_addr, ndev->addr_len);                                
            }else if(ex_nr==3)
            {
                memcpy(lp->ndev->dev_addr, exynos_3_mac_A_addr, ndev->addr_len);        
            }
            dbg1("kgex_probe(): wrote NIC A MAC Address for Exynos #%u\n", ex_nr);
            
            /*Network FPGA enpoint*/
            lp->tx_irq = 85;
            lp->rx_irq = 86;   
            dbg1("kgex_probe(): Got IRQs TX=%d  RX=%d\n", lp->tx_irq, lp->rx_irq);                        
        }else if (lp->id == ETH_ID_NIC_B)
        {         
            status = kgex_check_net_device(ETH_ID_NIC_A);
            if (status==1)
            {
                memcpy(ndev->dev_addr, exynos_1_mac_B_addr, ndev->addr_len); 
                dbg1("kgex_probe(): wrote NIC B MAC Address\n");                
                temp = netdev_priv(net_dev_tbl[ETH_ID_NIC_A]);                                                
            }                        
            
        }else if (lp->id == ETH_ID_NIC_C)
        {
            dma_phys_addr = BASE_NIC_C_DMA;
            dma_length = LEN_DMA_REGS;
            
            lp->dma_regs = ioremap(dma_phys_addr, dma_length);        
            if (lp->dma_regs == NULL)
            {
                    dev_err(&op->dev, "ERROR: cannot map DMA device\n");
                    return -ENODEV;
            }
            dbg1("kgex_probe(): ioremapped lp->dma_regs phys=0x%08llX virt=0x%016llX\n", dma_phys_addr, (uint64_t)lp->dma_regs);	            

            if (ex_nr==0)
            {
                memcpy(lp->ndev->dev_addr, exynos_0_mac_C_addr, ndev->addr_len);                                
            }else if (ex_nr==1)
            {
                memcpy(lp->ndev->dev_addr, exynos_1_mac_C_addr, ndev->addr_len);                                
            }else if(ex_nr==2)
            {
                memcpy(lp->ndev->dev_addr, exynos_2_mac_C_addr, ndev->addr_len);                                
            }else if(ex_nr==3)
            {
                memcpy(lp->ndev->dev_addr, exynos_3_mac_C_addr, ndev->addr_len);        
            }
            dbg1("kgex_probe(): wrote NIC A MAC Address for Exynos #%u\n", ex_nr);	 

            /*Storage FPGA endpoint*/
            lp->tx_irq = 53;       
            lp->rx_irq = 54;            
            dbg1("kgex_probe(): Got IRQs TX=%d  RX=%d\n", lp->tx_irq, lp->rx_irq);                        
        }else if (lp->id == ETH_ID_NIC_D)
        {
            //TODO: support for Network FPGA Management NIC (NIC D)
        }else
        {
            prerr("Unsupported ethernet interface ID.\n"); 
            return 1;
        }
                	                                                                                                         
        net_dev_tbl[lp->id] = ndev;
        num_net_devs += 1;         
                
	
        if (lp->id!=ETH_ID_NIC_B)
        {
#ifdef TX_THREADSAFE
            spin_lock_init(&lp->tx_lock);	
#endif
#ifdef RX_THREADSAFE        
            spin_lock_init(&lp->rx_lock);
#endif                  
            lp->coalesce_count_tx = tx_coalesce;
            pr("TX coalescing is %u.\n", lp->coalesce_count_tx);
            lp->coalesce_count_rx = rx_coalesce;
            pr("RX coalescing is %u.\n", lp->coalesce_count_rx);        
            
            if (jumbo!=0)
            {
                lp->jumbo_support = 1;
            }else
            {
                lp->jumbo_support = 0 ;
            }
        }
                
         
        if (lp->id!=ETH_ID_NIC_B)
        {   
            netif_napi_add(ndev, &lp->napi, kgex_napi_rx_poll, weight);            
            
            tasklet_init(&lp->dma_err_task, kgex_dma_err_handler, lp->id);
        
            if (tx_poll!=0)
            {
                tasklet_init(&lp->tx_poll_task, kgex_tx_poll, lp->id);
                pr("Initialized poll tasklet for TX.\n");
            }
            
            if(rx_poll!=0)
            {
                tasklet_init(&lp->rx_poll_task, kgex_rx_poll, lp->id);
                pr("Initialized poll tasklet for RX.\n");
            }  
        }      
                
        of_node_put(np);
        
        if(lp->id==ETH_ID_NIC_B)
        {            
            pr("Cloning Eth ID %u to Eth ID %u ...", temp->id, lp->id);
            lp = temp;
            pr("[  OK  ]\n");
        }

	pr("Network device initialized\n");

	dbg1("kgex_probe(): RETURN\n");
	return 0;
}

static int kgex_remove(struct platform_device *op)
{
	struct net_device *ndev;
        struct net_private *lp;
	dbg1("kgex_remove(): ENTER\n");

	ndev = platform_get_drvdata(op);
        
        if (ndev==NULL){prwarn("kgex_remove(): ndev==NULL\n");}
                
        
        if (ndev)
        {
            lp = netdev_priv(ndev);
            
            netif_napi_del(&lp->napi);            
            //netif_carrier_off(ndev);        
            //kgex_rings_free(ndev);                
            unregister_netdev(ndev);                        
            free_netdev(ndev);
            
//             dbg1("kgex_remove(): killing tasklets\n");
//             tasklet_kill(&(lp->dma_err_task));
//             tasklet_kill(&(lp->kgex_tx_poll));
            dbg1("kgex_remove(): iounmapping I/O space\n");
            if (lp->dma_regs) {iounmap(lp->dma_regs);}
            
            dev_set_drvdata(&op->dev, NULL);
            platform_set_drvdata(op, NULL);
        }                        
        
	pr("driver removed\n");
	dbg1("kgex_remove(): RETURN\n");
	return 0;
}


/*===== End of Platform Driver Functions =========*/


MODULE_AUTHOR("John Velegrakis, Kaleao Ltd.");
MODULE_DESCRIPTION("Kmax 10Gbps Ethenet driver");
MODULE_LICENSE("GPL");
