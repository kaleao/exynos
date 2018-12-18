///@cond DEV_DOC

/**
 * @file eth10g.c 10g Ethernet NIC driver - Main functions
 * @author John Velegrakis
 * 
 * @page eth10g
 */

#include "eth10g.h"

//#define DEBUG_FUNCTIONS

unsigned char default_mac_addr[] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x65};

extern uint64_t eusrvmem_get_adma_base_addr(void);
extern uint64_t eusrvmem_get_mac_base_addr(void);
extern int eusrvmem_get_admatx_irq(void);
extern int eusrvmem_get_admarx_irq(void);

static struct of_device_id eth10g_match[] = {
	{ .compatible = "kmax,kmax-ethernet-1.00.a,eth10g,kmax-eth10g", },
	{},	
};

MODULE_DEVICE_TABLE(of, eth10g_match);

/*========== Function Prototypes =============================================*/
//   Platform Driver Functions
static int eth10g_remove(struct platform_device *op);
static int eth10g_probe(struct platform_device *op);

static struct platform_driver eth10g_driver = {
	.probe		= eth10g_probe,
	.remove		= eth10g_remove,
	//.shutdown	= eth10g_shutdown,
	.driver = {
		.name	= "eth10g",
		.owner	= THIS_MODULE,
		.of_match_table = eth10g_match,
	},
};


module_platform_driver(eth10g_driver);

//   Network Device Functions
int eth10g_open(struct net_device *dev);
int eth10g_stop(struct net_device *dev);
static int eth10g_start_xmit (struct sk_buff *skb, struct net_device *dev);
static struct net_device_stats *eth10g_get_stats(struct net_device *dev);
static int eth10g_change_mtu(struct net_device *ndev, int new_mtu);
static int eth10g_set_mac_address(struct net_device *ndev, void *address);
static const struct net_device_ops eth10g_ops = {
	.ndo_open		= eth10g_open,
	.ndo_stop		= eth10g_stop,
	.ndo_start_xmit		= eth10g_start_xmit,
	.ndo_get_stats		= eth10g_get_stats,
	.ndo_set_mac_address    = eth10g_set_mac_address,
	.ndo_change_mtu		= eth10g_change_mtu,
};



/*For kernel 3.10*/
static void eth10g_dma_err_handler(unsigned long val);
/*For recent kernels*/
//static void eth10g_dma_err_handler(struct net_device *ndev);

// static void eth10g_dma_tx_reset(struct net_private *lp);
// static void eth10g_dma_rx_reset(struct net_private *lp);

//   Interrupt Handlers
static irqreturn_t eth10g_irq(int irq, void *_ndev);
static irqreturn_t eth10g_tx_irq(int irq, void *_ndev);
static irqreturn_t eth10g_rx_irq(int irq, void *_ndev);

//   Auxiliary Functions
#ifdef DEBUG_FUNCTIONS
	static int print_skb(struct sk_buff *skb, unsigned int len);
#endif

/*======== End of Function Prototypes ========================================*/


/*======== Auxiliary Functions ==================================*/
#ifdef DEBUG_FUNCTIONS
int print_skb(struct sk_buff *skb, unsigned int len)
{
	#define PBUFF_SIZE 3000
	unsigned int i = 0;	
	unsigned int index = 0;		
	char *pbuff = NULL;
	int bytes = 0;
	
	if (len<16) {return 0;}
	if (skb==NULL)
	{
		pr("===== skb =====\n");
		pr("     NULL      \n");
		pr("===============\n");
		return 0;
	}
	if (skb->data==NULL)
	{
		pr("===== skb =====\n");
		pr("   data NULL   \n");
		pr("===============\n");

	}
	
		
	pbuff = (char *)kmalloc(PBUFF_SIZE*sizeof(char), GFP_KERNEL);
	for(i=0; i<PBUFF_SIZE; i++)
	{
		pbuff[i] = ' ';
	}
	
	pr("===== skb =====\n");	
	for(i=0, index=0; i<len; i++)
	{		
		bytes = sprintf(&pbuff[index], "%.2X ", (uint32_t)(skb->data[i]));
		index += bytes;
		
		if ((i+1)%8==0 && i!=0)
		{
			if ((i+1)%16==0)
			{
				bytes = sprintf(&pbuff[index], "%c", '\n');
				index += bytes;
			}else
			{
				bytes = sprintf(&pbuff[index], "%c%c%c", ' ', ' ', ' ');
				index += bytes;
			}
			
		}

	}
	sprintf(&pbuff[index], "%c", '\0');
	pr("%s\n", pbuff);
	pr("===============\n");	
	kfree(pbuff);
	return 0;
}

static void eth10g_print_mac(struct net_device *ndev)
{
	int i;
	unsigned char *mac; 

	if (ndev == NULL) {printk(KERN_INFO "ndev = NULL\n");return;}

	mac = (unsigned char *)(ndev->dev_addr);

	printk(KERN_INFO "eeth10g: MAC: ");
	for (i=0; i<ETH_ALEN; i++)
	{
		printk(KERN_INFO "%02x ", mac[i]); 
	}
	printk(KERN_INFO "\n");
}
#endif
/*======== End of Auxiliary Functions ===========================*/

static inline int eth10g_check_tx_bd_space(struct net_private *lp, int num_frag)
{
	struct dma_bd *cur_p;
	cur_p = &lp->tx_bd_virt[(lp->tx_tail + num_frag) % TX_BD_NUM];
	if (cur_p->status & DMA_BD_STS_ALL_MASK)
		return NETDEV_TX_BUSY;
	return 0;
}

// static void eth10g_dma_tx_reset(struct net_private *lp)
// {
// 	uint32_t timeout = 0;
// 	
// 	dma_write32(lp, DMA_TX_CR_OFFSET, DMA_CR_RESET_MASK);
// 	timeout = 1000;	
// 	while(dma_read32(lp, DMA_TX_CR_OFFSET) & DMA_CR_RESET_MASK)
// 	{
// 		udelay(1);
// 		if (--timeout == 0) 
// 		{
// 			prerr("DMA TX reset timeout\n");
// 			break;
// 		}
// 	}		
// }

// static void eth10g_dma_rx_reset(struct net_private *lp)
// {
// 	uint32_t timeout = 0;
// 	dma_write32(lp, DMA_RX_CR_OFFSET, DMA_CR_RESET_MASK);
// 	timeout = 1000;
// 	while(dma_read32(lp, DMA_RX_CR_OFFSET) & DMA_CR_RESET_MASK)
// 	{
// 		udelay(1);
// 		if (--timeout == 0)
// 		{
// 			prerr("DMA RX reset timeout\n");
// 			break;
// 		}
// 	}	
// }

/**
 * eth10g_rings_init - Init rings of DMA descriptors and chain them
 * @ndv: pointer to network device
 * @retval status 0 on Ok
 */
static int eth10g_rings_init(struct net_device *ndev)
{
	int i = 0;
	int status = 0;
	uint32_t cr = 0;
#ifdef RX_ENABLE	
	struct sk_buff *skb = NULL;
#endif
	struct net_private *lp = NULL;
	uint64_t phys = 0;
	uint64_t temp = 0;
	

	dbg1("eth10g_rings_init()\n");

	lp = netdev_priv(ndev);

	lp->tx_curr = 0;
	lp->tx_tail = 0;
	lp->rx_curr = 0;
	
	//TODO: check DMA mask
	status = dma_set_coherent_mask(&(ndev->dev), 0xFFFFFFFFFF);
	dbg1("eth10g_rings_init(): dma_set_coherent_mask() status=%d\n", status);	

	//Allocate TX descriptors (DMA capable)
	lp->tx_bd_virt = dma_alloc_coherent(ndev->dev.parent, 
			sizeof(struct dma_bd) * TX_BD_NUM,
			&lp->tx_bd_phys,
			GFP_KERNEL);

	if (lp->tx_bd_virt == NULL)
	{
		prerr("cannot allocate TX BD ring\n");
		return -ENOMEM;
	} 

	dbg1(KERN_INFO "eth10g_rings_init(): lp->tx_bd_phys=0x%16llX\n", (uint64_t)lp->tx_bd_phys);
	
	//Allocate RX descriptors (DMA capable)
	lp->rx_bd_virt = dma_alloc_coherent(ndev->dev.parent,
			sizeof(struct dma_bd) * RX_BD_NUM,
			&lp->rx_bd_phys,
			GFP_KERNEL);

	if (lp->rx_bd_virt == NULL)
	{
		prerr("cannot allocate RX BD ring\n");
		return -ENOMEM;
	}

	memset(lp->tx_bd_virt, 0, sizeof(struct dma_bd) * TX_BD_NUM);
	memset(lp->rx_bd_virt, 0, sizeof(struct dma_bd) * RX_BD_NUM);
	
	dbg1("Chaining TX descriptors\n");
	dbg1("lp->rx_bd_phys=0x%16llX\n", (uint64_t)lp->rx_bd_phys);	
	/*Create linked-list of TX BDs*/
	for (i=0; i<TX_BD_NUM; i++)
	{//	dbg1("CPU addr=0x%llX\n", lp->tx_bd_phys + sizeof(struct dma_bd) * ((i+1)%TX_BD_NUM));
		temp = addr_trans(lp->tx_bd_phys + sizeof(struct dma_bd) * ((i+1)%TX_BD_NUM));	
	//	dbg1("   translated = 0x%llX ,   LO32=0x%08lX, HI32=0x%08lX\n", temp, LO32(temp), HI32(temp));
		lp->tx_bd_virt[i].next_lo = LO32(temp);
		lp->tx_bd_virt[i].next_hi = HI32(temp);
	}
	
#ifdef RX_ENABLE	
	/*Create linked-list and init RX BDs*/
	dbg1("Chaining RX descriptors\n");
	for (i=0; i<RX_BD_NUM; i++)
	{
		//Create physical linked-list	
		temp = addr_trans(lp->rx_bd_phys + sizeof(struct dma_bd) * ((i+1)%RX_BD_NUM));				
		lp->rx_bd_virt[i].next_lo = LO32(temp);
		lp->rx_bd_virt[i].next_hi = HI32(temp);
		
		////skb = dev_alloc_skb(lp->max_frm_size+2);		
		skb = netdev_alloc_skb_ip_align(ndev, lp->max_frm_size);		
		////skb = netdev_alloc_skb(ndev, lp->max_frm_size);
		if (!skb){prerr("cannot allocate RX skb No. #%u\n", i);goto out;}		
		//DBG3 printk(KERN_INFO "eth10g_rings_init(): skb=0x%X\n", (uint32_t)skb);		
		
		//Give the pointer of the skb to the BD		
		lp->rx_bd_virt[i].rx_skb_virt_lo = LO32((uint64_t) skb);
		lp->rx_bd_virt[i].rx_skb_virt_hi = HI32((uint64_t) skb);
				
		phys = dma_map_single(ndev->dev.parent, skb->data, lp->max_frm_size, DMA_FROM_DEVICE);		
		if (dma_mapping_error(ndev->dev.parent, phys)) {prerr("cannot DMA map RX skb No. #%u\n", i); }		
		
		temp = addr_trans(phys);		
		lp->rx_bd_virt[i].phys_lo = LO32(temp);
		lp->rx_bd_virt[i].phys_hi = HI32(temp);		

		lp->rx_bd_virt[i].phys_cpu_lo = LO32(phys);
		lp->rx_bd_virt[i].phys_cpu_hi = HI32(phys);
		
		lp->rx_bd_virt[i].cntrl = lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK | DMA_BD_CTRL_TXEOF_MASK;
	}
	dbg1("Chained RX descriptors\n");			
#endif	

#ifdef RX_ENABLE
	/*Init RX part*/	
	temp = addr_trans(lp->rx_bd_phys);
	dbg1("Writing DMA RX current descriptor register\n");
	dma_write32(lp, DMA_RX_CDESC_OFFSET_LO, LO32(temp));
	dma_write32(lp, DMA_RX_CDESC_OFFSET_HI, HI32(temp));			
	
	cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	cr = ((cr & ~DMA_COALESCE_MASK) |
	((lp->coalesce_count_rx) << DMA_COALESCE_SHIFT));
	cr = ((cr & ~DMA_DELAY_MASK) |
	(DMA_DFT_RX_WAITBOUND << DMA_DELAY_SHIFT));	
	
	cr |= DMA_IRQ_ALL_MASK;
	
	dbg1("Writing DMA RX control register\n");
	dma_write32(lp, DMA_RX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
	
	temp = addr_trans(lp->rx_bd_phys + (sizeof(struct dma_bd) * (RX_BD_NUM -1)));
	dbg1("Writing DMA RX tail descriptor register\n");
	dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, LO32(temp));
	dma_write32(lp, DMA_RX_TDESC_OFFSET_HI, HI32(temp));
#endif
	
#ifdef TX_ENABLE
	/*Init TX part*/
	temp = addr_trans(lp->tx_bd_phys);
	dbg1("Writing TX Channel current descriptor registers\n");
	dma_write32(lp, DMA_TX_CDESC_OFFSET_LO, LO32(temp));   			
	dma_write32(lp, DMA_TX_CDESC_OFFSET_HI, HI32(temp));   					

	cr = dma_read32(lp, DMA_TX_CR_OFFSET);
	cr = ((cr & ~DMA_COALESCE_MASK) |
	((lp->coalesce_count_tx) << DMA_COALESCE_SHIFT));
	cr = ((cr & ~DMA_DELAY_MASK) |
	(DMA_DFT_TX_WAITBOUND << DMA_DELAY_SHIFT));		
	
	cr |=  DMA_IRQ_ALL_MASK;

	dbg1("Writing TX Channel Control register\n");
	dma_write32(lp, DMA_TX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);

#endif	
	pr("Initialized %u TX and %u RX descriptors", TX_BD_NUM, RX_BD_NUM);
	dbg1("eth10g_rings_init(): return 0;\n");
	return 0;
	
out:	
	//TODO: release bd rings
	printk(KERN_INFO "eth10g_rings_init(): return -ENOMEM;\n");
	return -ENOMEM;
}


static int eth10g_rings_free(struct net_device *ndev)
{
	int i = 0;		
	uint64_t phys = 0;
	struct net_private *lp = NULL;
	struct sk_buff *skb = NULL;
	
	dbg1("eth10g_rings_free(): ENTER\n");
	lp = netdev_priv(ndev);

	for(i=0; i<RX_BD_NUM;i++)
	{
		phys = WHOLE(lp->rx_bd_virt[i].phys_cpu_lo, lp->rx_bd_virt[i].phys_cpu_hi);
		if (phys) {dma_unmap_single(ndev->dev.parent, phys,lp->max_frm_size, DMA_FROM_DEVICE); }
		skb = (struct sk_buff *)WHOLE(lp->rx_bd_virt[i].rx_skb_virt_lo, lp->rx_bd_virt[i].rx_skb_virt_hi);		
		if (skb) {dev_kfree_skb(skb);}
	}
	
	if (lp->rx_bd_virt)
	{
		dma_free_coherent(ndev->dev.parent, 
				  sizeof(*lp->rx_bd_virt) * RX_BD_NUM, 
				  lp->rx_bd_virt, 
				  lp->rx_bd_phys);
	}
	
	if (lp->tx_bd_virt)
	{
		dma_free_coherent(ndev->dev.parent, 
				  sizeof(*lp->tx_bd_virt) * TX_BD_NUM, 
				  lp->tx_bd_virt, 
				  lp->tx_bd_phys);
	}
	pr("Rings of descriptors released\n");
	dbg1("eth10g_rings_free(): RETURN\n");		
	return 0;
}

/**
 * eth10g_start_xmit - Transmit an ethernet frame (called by the kernel)
 * @skb: socket buffer
 * @ndev: network device
 * return NETDEV_TX_OK on success, other if error or busy
 **/
static int eth10g_start_xmit (struct sk_buff *skb, struct net_device *ndev)
{
	int i;
	struct net_private *lp;			
	uint32_t num_frag;
	struct dma_bd *cur_p;
	skb_frag_t *frag;
	dma_addr_t tail_phys;
	uint64_t phys = 0;
	uint64_t temp = 0;
	
	//unsigned long flags;
				
	dbg1("eth10g_start_xmit(): ENTER\n");	

	lp = netdev_priv(ndev);	
	
	//if ((skb->len<=0)){prerr("eth10g-eth: ERROR: got TX skb with len<=0\n");dev_kfree_skb_any(skb);return NETDEV_TX_OK;}
	
	num_frag = skb_shinfo(skb)->nr_frags;
				
	//DEBUG
	//if (num_frag>0) {pr("eth10g-eth: TX frags=%u\n", num_frag);}
	
	if (eth10g_check_tx_bd_space(lp, num_frag))
	{		
		prerr("NETDEV_TX_BUSY\n");
		if (!netif_queue_stopped(ndev)){netif_stop_queue(ndev);}		
		return NETDEV_TX_BUSY;		
	}		
	
	//DEBUG
	//if (spin_is_locked(&lp->tx_tail_lock)){	return NETDEV_TX_BUSY;}
#ifdef TX_THREADSAFE
	spin_lock_irq(&lp->tx_curr_lock);
#endif
	
	cur_p = &lp->tx_bd_virt[lp->tx_tail];
	cur_p->cntrl = skb_headlen(skb) | DMA_BD_CTRL_TXSOF_MASK;	
	phys = dma_map_single(ndev->dev.parent, skb->data, skb_headlen(skb), DMA_TO_DEVICE);	

#ifdef TX_CHECK_UNALIGNED
	if ((phys % 0x4) != 0)
	{
		lp->tx_unaligned32++;
	}
#endif	
	
	temp = addr_trans(phys);	
	cur_p->phys_lo = LO32(temp);
	cur_p->phys_hi = HI32(temp);	
	cur_p->phys_cpu_lo = LO32(phys);
	cur_p->phys_cpu_hi = HI32(phys);
	
//	mlkia = WHOLE(cur_p->phys_cpu_lo, cur_p->phys_cpu_hi);	
//	dbg1("Frag CPU addr=0x%llX  (0x%llX)\n", phys,  mlkia);
//	mlkia = WHOLE(cur_p->phys_lo, cur_p->phys_hi);
//	dbg1("   translated addr=0x%llX    (0x%llX)\n", temp,  mlkia);	
			
	for (i=0; i<num_frag; i++)
	{
		++lp->tx_tail;
		lp->tx_tail %= TX_BD_NUM;				
		cur_p = &lp->tx_bd_virt[lp->tx_tail];
		frag = &skb_shinfo(skb)->frags[i];
		phys = dma_map_single(ndev->dev.parent,
					     skb_frag_address(frag),
					     skb_frag_size(frag),
					     DMA_TO_DEVICE);
		
		if(dma_mapping_error(ndev->dev.parent, temp)){prerr("dma_mapping_error\n");spin_unlock_irq(&lp->rx_lock);}
		temp = addr_trans(phys);
		cur_p->phys_lo = LO32(temp);
		cur_p->phys_hi = HI32(temp);

		cur_p->phys_cpu_lo = LO32(phys);
		cur_p->phys_cpu_hi = HI32(phys);
		
		
		dbg3("eth10g_start_xmit(): cur_p->cntrl = skb_frag_size(frag) = %u, cur_p->phys=0x%X\n", skb_frag_size(frag), (unsigned long)cur_p->phys);
		cur_p->cntrl = 0x0 | skb_frag_size(frag);
		//DEBUG
		//if (cur_p->cntrl > lp->max_frm_size){pr("eth10g-eth: TX packet length = %u\n", cur_p->cntrl)};		
	}
	
	cur_p->cntrl |= DMA_BD_CTRL_TXEOF_MASK;
	cur_p->tx_skb_virt_lo = LO32((uint64_t)skb);
	cur_p->tx_skb_virt_hi = HI32((uint64_t)skb);

	/*Find last BD physical address*/
	tail_phys = lp->tx_bd_phys + sizeof(struct dma_bd) * lp->tx_tail;
	wmb();
	
	/*Start DMA transfer*/	
	temp = addr_trans(tail_phys);
	dma_write32(lp, DMA_TX_TDESC_OFFSET_LO, LO32(temp));
	dma_write32(lp, DMA_TX_TDESC_OFFSET_HI, HI32(temp));
	//hw_write(lp->dma_regs, DMA_TX_TDESC_OFFSET, tail_phys);		
	
	/*Compute next new tail*/
	++(lp->tx_tail);
	lp->tx_tail %= TX_BD_NUM;
	
	lp->tx_coa_frags++;
	
	dbg1("eth10g_start_xmit(): RETURN\n");	
#ifdef TX_THREADSAFE	
	spin_unlock_irq(&lp->tx_curr_lock);
#endif
	return NETDEV_TX_OK;
}

/**
 * eth10g_start_xmit_done - Called by the interrupt handler. Updares descriptors
 * and indexes.
 * @ndev: network device 
 **/
static void eth10g_start_xmit_done(struct net_device *ndev)
{
	unsigned int status = 0;	
	uint32_t size = 0;
	uint64_t packets = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd;	
	//uint64_t phys_cpu = 0;
	uint64_t temp = 0;

	dbg1("eth10g_start_xmit_done():\n");	

	lp = netdev_priv(ndev);	

	lp->tx_coa_frags--;
	
	
	cur_bd = &lp->tx_bd_virt[lp->tx_curr];
	status = cur_bd->status;
	while ((status & DMA_BD_STS_COMPLETE_MASK) || (status & DMA_BD_STS_ALL_ERR_MASK)) 		
	{			
		dbg1("eth10g_start_xmit_done(): DMA_BD_STS_COMPLETE_MASK lp->tx_curr=%u\n", (uint32_t)(lp->tx_curr));
		if (status & DMA_BD_STS_DEC_ERR_MASK){prerr("DMA RX descriptor DMA_BD_STS_DEC_ERR_MASK\n");}						
		if (status & DMA_BD_STS_INT_ERR_MASK){prerr("DMA RX descriptor DMA_BD_STS_INT_ERR_MASK\n");}
		if (status & DMA_BD_STS_SLV_ERR_MASK){prerr("DMA RX descriptor DMA_BD_STS_SLV_ERR_MASK\n");}				
		
		
		temp = WHOLE(cur_bd->phys_lo, cur_bd->phys_hi);
		
		dma_unmap_single(ndev->dev.parent, temp, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_TO_DEVICE);		
		if (cur_bd->tx_skb_virt_lo || cur_bd->tx_skb_virt_hi)
		{
			dbg3("eth10g_start_xmit_done(): cur_bd->rx_skb_virt!=NULL\n");
			temp = WHOLE(cur_bd->tx_skb_virt_lo, cur_bd->tx_skb_virt_hi);
			dev_kfree_skb_irq((struct sk_buff *)temp);
		}
		/*cur_p->phys = 0;*/
		cur_bd->app0 = 0;		
		cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
		cur_bd->status = 0;
		
		dbg2("eth10g_start_xmit_done(): size=%u\n", (uint32_t)(status & DMA_BD_STS_ACTUAL_LEN_MASK));

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
}

int eth10g_recv(struct net_device *ndev)
{		
		unsigned int packets = 0;
		unsigned int len = 0;
		unsigned int size = 0;
		dma_addr_t tail_phys = 0;
		struct net_private *lp = NULL;
		struct sk_buff *skb, *new_skb;
		struct dma_bd *cur_bd;
		int netrx = 0;
		unsigned int i = 0;	
		uint64_t phys_cpu = 0;
		uint64_t temp = 0;				
		
		dbg1("eth10g_recv(): ENTER\n");		
		lp = netdev_priv(ndev);
		
		
		cur_bd = &lp->rx_bd_virt[lp->rx_curr];
		rmb();
						
		while((cur_bd->status & DMA_BD_STS_COMPLETE_MASK) || (cur_bd->status & DMA_BD_STS_ALL_ERR_MASK))
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
			dbg2("eth10g_recv(): len=%u\n", len);	
			//DEBUG
			if (len>lp->max_frm_size){pr("RX packet length = %u\n", len);}
							
			//dma_unmap_single(ndev->dev.parent, cur_bd->phys, lp->max_frm_size, DMA_FROM_DEVICE);
			skb->dev = ndev;
			skb_put(skb, len);
			skb->protocol = eth_type_trans(skb, ndev);			
			skb->ip_summed = CHECKSUM_UNNECESSARY;								
			
			netrx = netif_rx(skb);
			////netrx = netif_receive_skb(skb);			
			if (netrx == NET_RX_SUCCESS)
			{
				dbg1("eth10g_recv(): NET_RX_SUCCESS, #%u, status=0x%X\n", i, cur_bd->status);								
			}else
			{
				prerr("NET_RX_DROP, #%u, status=0x%X\n", i, cur_bd->status);												
			}			
			size += len;
			packets ++;
			
			////new_skb = dev_alloc_skb(lp->max_frm_size+2);
			new_skb = netdev_alloc_skb_ip_align(ndev, lp->max_frm_size);
			////new_skb = netdev_alloc_skb(ndev, lp->max_frm_size);
			if (new_skb==NULL)
			{
				prerr("Cannot allocate new RX skb\n");				
				return 1;
			}
			
			rmb();
			
			phys_cpu = dma_map_single(ndev->dev.parent, new_skb->data, lp->max_frm_size, DMA_FROM_DEVICE);
			
			temp = addr_trans(phys_cpu);
			cur_bd->phys_lo = LO32(temp);
			cur_bd->phys_hi = HI32(temp);			
			cur_bd->phys_cpu_lo = LO32(phys_cpu);
			cur_bd->phys_cpu_hi = HI32(phys_cpu);
			
			cur_bd->cntrl = lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK |DMA_BD_CTRL_TXEOF_MASK;
			cur_bd->status = 0x0;
			cur_bd->rx_skb_virt_lo = LO32((uint64_t)new_skb);
			cur_bd->rx_skb_virt_hi = HI32((uint64_t)new_skb);
			
			++lp->rx_curr;
			lp->rx_curr %= RX_BD_NUM;
			cur_bd = &lp->rx_bd_virt[lp->rx_curr];
		}
		
		ndev->stats.rx_packets += packets;
		ndev->stats.rx_bytes += size;
		
		//if (tail_phys)
		//{
		temp = addr_trans(tail_phys);
		dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, LO32(temp));
		dma_write32(lp, DMA_RX_TDESC_OFFSET_HI, HI32(temp));		
		//}else
		//{
		//	prerr("tail physical address is 0x0\n");
		//}
		
		dbg1("eth10g_recv(): RETURN\n");				
		
		return 0;
}

/*For recent kernels*/
// static void eth10g_dma_err_handler(struct net_device *ndev)
// {
// 	unsigned int i = 0;
// 	unsigned int timeout = 1000; //1 msec timeout	
// 	struct net_private *lp = NULL;
// 	struct dma_bd *cur_bd = NULL;
// 	uint64_t phys_cpu = 0;
// 	uint64_t temp = 0;
// 	
// 	lp = netdev_priv(ndev);
// 	
// 	pr("DMA error handler tasklet\n");
// 	/*Reset DMA TX path*/
// 	dma_write32(lp, DMA_TX_CR_OFFSET, DMA_CR_RESET_MASK);
// 	while (dma_read32(lp, DMA_TX_CR_OFFSET) & DMA_CR_RESET_MASK)	
// 	{
// 		udelay(1);
// 		if (--timeout == 0)
// 		{
// 			prerr("DMA TX reset timeout occured!\n");
// 			break;
// 		}
// 	}
// 	
// 	/*Reset DMA RX path*/
// 	dma_write32(lp, DMA_RX_CR_OFFSET, DMA_CR_RESET_MASK);
// 	while (dma_read32(lp, DMA_RX_CR_OFFSET) & DMA_CR_RESET_MASK)	
// 	{
// 		udelay(1);
// 		if (--timeout == 0)
// 		{
// 			prerr("DMA RX reset timeout occured!\n");
// 			break;
// 		}
// 	}
// 
// 	/*Reset TX BD ring*/
// 	for(i=0; i<TX_BD_NUM; i++)
// 	{
// 		cur_bd = &lp->tx_bd_virt[i];
// 		if (cur_bd->phys_cpu_lo || cur_bd->phys_cpu_hi)
// 		{
// 			phys_cpu = WHOLE(cur_bd->phys_cpu_lo, cur_bd->phys_cpu_hi);
// 			dma_unmap_single(ndev->dev.parent, phys_cpu, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_TO_DEVICE);			
// 		}
// 		if (cur_bd->rx_skb_virt_lo || cur_bd->rx_skb_virt_hi)
// 		{
// 			temp = WHOLE(cur_bd->rx_skb_virt_lo, cur_bd->rx_skb_virt_hi);
// 			dev_kfree_skb_irq((struct sk_buff *)temp);			
// 		}
// 		cur_bd->phys_lo = 0;
// 		cur_bd->phys_hi = 0;
// 		cur_bd->phys_cpu_lo = 0;
// 		cur_bd->phys_cpu_hi = 0;
// 		cur_bd->cntrl = 0;
// 		cur_bd->status=0;
// 		cur_bd->app0 = 0;
// 		cur_bd->rx_skb_virt_lo = 0;
// 		cur_bd->rx_skb_virt_hi = 0;			
// 		cur_bd->app3 = 0;
// 		cur_bd->tx_skb_virt_lo = 0;
// 		cur_bd->tx_skb_virt_hi = 0;
// 	}
// 	
// #ifdef RX_ENABLE		
// 	/*Reset RX BD ring*/
// 	for(i=0; i<RX_BD_NUM; i++)
// 	{
// 		cur_bd = &lp->rx_bd_virt[i];
// 		cur_bd->status = 0;
// 		cur_bd->app0 = 0;
// 		cur_bd->rx_skb_virt_lo = 0;
// 		cur_bd->rx_skb_virt_hi = 0;
// 		cur_bd->app3 = 0;
// 		cur_bd->tx_skb_virt_lo = 0;
// 		cur_bd->tx_skb_virt_hi = 0;
// 	}
// #endif
// 
// 	lp->tx_curr = 0;
// 	lp->tx_tail = 0;
// 	lp->rx_tail = 0;
// 
// #ifdef RX_ENABLE
// 	temp = addr_trans(lp->rx_bd_phys);
// 	dma_write32(lp, DMA_RX_CDESC_OFFSET_LO, LO32(temp));	
// 	dma_write32(lp, DMA_RX_CDESC_OFFSET_HI, HI32(temp));	
// 	//hw_write(lp->dma_regs, DMA_RX_CR_OFFSET, 0x00001001);
// 	hw_write(lp->dma_regs, DMA_RX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
// 	
// 	temp = addr_trans(lp->rx_bd_phys + (sizeof(struct dma_bd) * (RX_BD_NUM -1)));
// 	
// 	dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, LO32(temp));
// 	dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, HI32(temp));
// #endif	
// 	
// #ifdef TX_ENABLE
// 	temp = addr_trans(lp->tx_bd_phys);		
// 	dma_write32(lp, DMA_TX_CDESC_OFFSET_LO, LO32(temp));   			
// 	dma_write32(lp, DMA_TX_CDESC_OFFSET_HI, HI32(temp));   			
// 	hw_write(lp->dma_regs, DMA_TX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
// //	hw_write(lp->dma_regs, DMA_TX_CR_OFFSET, 0x00001001);
// #endif				
// }

/*For kernel 3.10*/
static void eth10g_dma_err_handler(unsigned long val)
{
	unsigned int i = 0;
	unsigned int timeout = 1000; //1 msec timeout	
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd = NULL;
	uint64_t phys_cpu = 0;
	uint64_t temp = 0;
	
        ndev = (struct net_device *)val;        
	lp = netdev_priv(ndev);
	
	pr("DMA error handler tasklet\n");
	/*Reset DMA TX path*/
	dma_write32(lp, DMA_TX_CR_OFFSET, DMA_CR_RESET_MASK);
	while (dma_read32(lp, DMA_TX_CR_OFFSET) & DMA_CR_RESET_MASK)	
	{
		udelay(1);
		if (--timeout == 0)
		{
			prerr("DMA TX reset timeout occured!\n");
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
			prerr("DMA RX reset timeout occured!\n");
			break;
		}
	}

	/*Reset TX BD ring*/
	for(i=0; i<TX_BD_NUM; i++)
	{
		cur_bd = &lp->tx_bd_virt[i];
		if (cur_bd->phys_cpu_lo || cur_bd->phys_cpu_hi)
		{
			phys_cpu = WHOLE(cur_bd->phys_cpu_lo, cur_bd->phys_cpu_hi);
			dma_unmap_single(ndev->dev.parent, phys_cpu, (cur_bd->cntrl & DMA_BD_CTRL_LENGTH_MASK), DMA_TO_DEVICE);			
		}
		if (cur_bd->rx_skb_virt_lo || cur_bd->rx_skb_virt_hi)
		{
			temp = WHOLE(cur_bd->rx_skb_virt_lo, cur_bd->rx_skb_virt_hi);
			dev_kfree_skb_irq((struct sk_buff *)temp);			
		}
		cur_bd->phys_lo = 0;
		cur_bd->phys_hi = 0;
		cur_bd->phys_cpu_lo = 0;
		cur_bd->phys_cpu_hi = 0;
		cur_bd->cntrl = 0;
		cur_bd->status=0;
		cur_bd->app0 = 0;
		cur_bd->rx_skb_virt_lo = 0;
		cur_bd->rx_skb_virt_hi = 0;			
		cur_bd->app3 = 0;
		cur_bd->tx_skb_virt_lo = 0;
		cur_bd->tx_skb_virt_hi = 0;
	}
	
#ifdef RX_ENABLE		
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
#endif

	lp->tx_curr = 0;
	lp->tx_tail = 0;
	lp->rx_tail = 0;

#ifdef RX_ENABLE
	temp = addr_trans(lp->rx_bd_phys);
	dma_write32(lp, DMA_RX_CDESC_OFFSET_LO, LO32(temp));	
	dma_write32(lp, DMA_RX_CDESC_OFFSET_HI, HI32(temp));	
	//hw_write(lp->dma_regs, DMA_RX_CR_OFFSET, 0x00001001);
	hw_write(lp->dma_regs, DMA_RX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
	
	temp = addr_trans(lp->rx_bd_phys + (sizeof(struct dma_bd) * (RX_BD_NUM -1)));
	
	dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, LO32(temp));
	dma_write32(lp, DMA_RX_TDESC_OFFSET_LO, HI32(temp));
#endif	
	
#ifdef TX_ENABLE
	temp = addr_trans(lp->tx_bd_phys);		
	dma_write32(lp, DMA_TX_CDESC_OFFSET_LO, LO32(temp));   			
	dma_write32(lp, DMA_TX_CDESC_OFFSET_HI, HI32(temp));   			
	hw_write(lp->dma_regs, DMA_TX_CR_OFFSET, DMA_IRQ_ALL_MASK | DMA_CR_RUNSTOP_MASK);
//	hw_write(lp->dma_regs, DMA_TX_CR_OFFSET, 0x00001001);
#endif				
}


/*========== Interrupt Handlers ==================*/
static irqreturn_t eth10g_irq(int irq, void *_ndev)
{
	dbg1("eth10g_irq(): ENTER\n");
	dbg1("eth10g_irq(): RETURN\n");
	return IRQ_HANDLED;
}


static irqreturn_t eth10g_tx_irq(int irq, void *_ndev)
{
	unsigned int status = 0;	
	struct net_device *ndev;
	struct net_private *lp;
	uint32_t cr = 0;	
#ifdef TX_THREADSAFE	
	unsigned long flags;
#endif	
	dbg1("eth10g_tx_irq(): ENTER\n");
	
	ndev = _ndev;
	lp = netdev_priv(ndev);	
#ifdef TX_THREADSAFE
	//spin_lock(&lp->lock);
	spin_lock_irqsave(&lp->tx_curr_lock, flags);
#endif	
	status = dma_read32(lp, DMA_TX_SR_OFFSET);
	dbg2("eth10g_tx_irq(): status = 0x%X\n", status);		
	//dma_write32(lp, DMA_TX_SR_OFFSET, status);						
	
	if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK))
	{
		dma_write32(lp, DMA_TX_SR_OFFSET, status);	
		eth10g_start_xmit_done(lp->ndev);
#ifdef TX_THREADSAFE		
		spin_unlock_irqrestore(&lp->tx_curr_lock, flags);
#endif		
		return IRQ_HANDLED;
	}
	
	if (status & DMA_IRQ_ERROR_MASK) 
	{
		prerr( "TX DMA error, status=0x%X\n", status);				
		
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

	dbg1("eth10g_tx_irq(): RETURN\n");
#ifdef TX_THREADSAFE	
	spin_unlock_irqrestore(&lp->tx_curr_lock, flags);
#endif
	return IRQ_HANDLED;
}

static irqreturn_t eth10g_rx_irq(int irq, void *_ndev)
{
	struct net_device *ndev;
	struct net_private *lp;	
	unsigned int status = 0;	
#ifdef RX_THREADSAFE	
	unsigned long flags;
#endif
	
	dbg1("eth10g_rx_irq(): ENTER\n");
	
	ndev = _ndev;
	lp = netdev_priv(ndev);	
#ifdef RX_THREADSAFE	
	spin_lock_irqsave(&lp->rx_lock, flags);
#endif					
	//status = ReadReg(lp->dma_regs, DMA_RX_SR_OFFSET);
	status = dma_read32(lp, DMA_RX_SR_OFFSET);
	if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK)) 
	{	
		dma_write32(lp, DMA_RX_SR_OFFSET, status);			
		eth10g_recv(ndev);				
		goto out_normal;
	}
	
	if (status & DMA_IRQ_ERROR_MASK)
	{
		dma_write32(lp, DMA_RX_SR_OFFSET, status);	
		prerr("RX DMA error, status=0x%X\n", status);		
		tasklet_schedule(&lp->dma_err_task);
	}

out_normal:
	dbg1("eth10g_rx_irq(): RETURN\n");
#ifdef RX_THREADSAFE	
	spin_unlock_irqrestore(&lp->rx_lock, flags);		
#endif	
	return IRQ_HANDLED;
}
/*========== End of  Interrupt Handlers ==========*/

/*========== Network Device Functions ====*/
int eth10g_open(struct net_device *ndev)
{
	int status = -1;
	struct net_private *lp;	

	dbg1("eth10g_open(): ENTER\n");

	lp = netdev_priv(ndev);	
	lp->cnt = 0;
	
////	lp->max_frm_size = ETH10G_MAX_VLAN_FRAME_SIZE;
////	
////	if(ndev->mtu > ETH10G_MTU)
////	{
////		lp->max_frm_size = ndev->mtu + VLAN_ETH_HLEN + ETH10G_TRL_SIZE;		
	////		printk(KERN_INFO "eth10g_open(): Resetting max frame size to %u bytes\m", lp->max_frm_size);
////	}
	lp->tx_coa_frags = 0;
	lp->rx_coa_frags = 0;
	lp->max_frm_size = ETH10G_MAX_FRM_SIZE;		
	pr("Set max frame size to %u bytes\n", lp->max_frm_size);
	
	if (ndev==NULL) {prerr("eth10g_open(): ndev is NULL");}
	
	status = eth10g_rings_init(ndev);
	if (status!=0){return status;}

	tasklet_init(&lp->dma_err_task, eth10g_dma_err_handler, (unsigned long)ndev);
	
	dbg1("eth10g_open(): TX irq = %d, RX irq = %d\n", lp->tx_irq, lp->rx_irq);	
	
	if (lp->irq != 0)
	{
		status = request_irq(lp->irq, eth10g_irq, 0, ndev->name, ndev);
		if (status){prerr("Cannot get combined TX/RX IRQ\n");return -ENOMEM;}						
	}else
	{			
		status = request_irq(lp->tx_irq, eth10g_tx_irq, 0, ndev->name, ndev);	
		if (status){prerr("Cannot get TX IRQ\n");return -ENOMEM;}						
		status = request_irq(lp->rx_irq, eth10g_rx_irq, 0, ndev->name, ndev);
		if (status){prerr("Cannot get RX IRQ\n");return -ENOMEM;}
	}
//	irq_set_irq_type(lp->tx_irq, IRQ_TYPE_EDGE_RISING);
//	irq_set_irq_type(lp->rx_irq, IRQ_TYPE_EDGE_RISING);

	//TODO: enable proc subsystem
#ifdef ENABLE_PROC	
	create_proc_eth10g(ndev);
#endif
	netif_start_queue(ndev);
	
	pr("Opened successfully!\n");
	dbg1("eth10g_open(): RETURN\n");
	return 0;
}

int eth10g_stop(struct net_device *ndev)
{
	struct net_private *lp = NULL;
	uint32_t cr = 0;
	
	if (ndev==NULL){prerr("ndev is NULL");return -1;}
	lp = netdev_priv(ndev);
		
	if (!netif_queue_stopped(ndev)){netif_stop_queue(ndev);}	
		
	cr = dma_read32(lp, DMA_TX_CR_OFFSET);
	dma_write32(lp, DMA_TX_CR_OFFSET, cr & (~DMA_CR_RUNSTOP_MASK));
	
	cr = dma_read32(lp, DMA_RX_CR_OFFSET);
	dma_write32(lp, DMA_TX_CR_OFFSET, cr & (~DMA_CR_RUNSTOP_MASK));

	tasklet_kill(&lp->dma_err_task);
	
	free_irq(lp->tx_irq, ndev);
	free_irq(lp->rx_irq, ndev);
	
	eth10g_rings_free(ndev);
#ifdef ENABLE_PROC		
	remove_proc_eth10g(ndev);
#endif
	
	pr("Stopped\n");
	
	return 0;
}

static struct net_device_stats *eth10g_get_stats(struct net_device *ndev)
{	
	return &ndev->stats;	
}

static int eth10g_change_mtu(struct net_device *ndev, int new_mtu)
{
	struct net_private *lp = netdev_priv(ndev);

//JVELEG: DBUG
	prerr("not supported yet\n");
	return 0;

	if (netif_running(ndev))
		return -EBUSY;
	if (lp->jumbo_support) {
		if ((new_mtu > ETH10G_JUMBO_MTU) || (new_mtu < 64))
			return -EINVAL;
		ndev->mtu = new_mtu;
	} else {
		if ((new_mtu > ETH10G_MTU) || (new_mtu < 64))
			return -EINVAL;
		ndev->mtu = new_mtu;
	}

	return 0;
}

static int eth10g_set_mac_address(struct net_device *ndev, void *address)
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
		pr("Invalid MAC adddress, setting random one.\n");
			eth_random_addr(ndev->dev_addr);
		}else
		{
			pr("Copying mac\n");
			memcpy(ndev->dev_addr, mac, ndev->addr_len);		
		}
	}
//		memcpy(ndev->dev_addr, mac, ETH_ALEN);

	return 0;
}


/*========== End of Network Device Functions ====*/

/*===== Platform Driver Functions ===============*/
/*int eth10g_init (struct net_device **dev_out)
{
	pr("init\n");	
	return 0;
}
*/
static int eth10g_probe(struct platform_device *op)
{
	int status;	
	//TODO: what is this?
	//struct device_node *np;	
	struct net_device *ndev;
	struct net_private *lp;
//	void *addr;	
	phys_addr_t mac_phys_addr;
	phys_addr_t mac_length;
	phys_addr_t dma_phys_addr;
	phys_addr_t dma_length;

	dbg1("eth10g_probe(): ENTER\n");	

	mac_phys_addr = 0x0;
	mac_length = 0x0; 
	
	dma_phys_addr = 0x0;
	dma_length = 0x0;
		

	ndev = alloc_etherdev(sizeof(struct net_private));
	if (!ndev)
	{
		prerr("Could not allocate ethernet device\n");
		return -1;
	}
	//dev_set_drvdata(&op->dev, ndev);
	SET_NETDEV_DEV(ndev, &op->dev);
	//ether_setup(ndev);

	ndev->netdev_ops = &eth10g_ops;
	//ndev->ethtool_ops = &eth10g_ethtool_ops;
        
#ifdef ENABLE_ETHTOOL        
	eth10g_ethtool_init(ndev);
#endif        
        
//	ndev->hard_header_len = 14;
	ndev->addr_len = ETH_ALEN; //added recently (usable?)
	//ndev->flags &= ~IFF_MULTICAST;  /* clear multicast */
	//ndev->flags |= IFF_NOARP;
	

	ndev->features = NETIF_F_SG;
	
	strcpy(ndev->name, "eth%d");

	lp = netdev_priv(ndev);
	lp->ndev = ndev;
	lp->dev = &op->dev;

#ifdef TX_THREADSAFE	
	spin_lock_init(&lp->tx_tail_lock);
	spin_lock_init(&lp->tx_curr_lock);
#endif
#ifdef RX_THREADSAFE	
	spin_lock_init(&lp->rx_lock);
#endif

	status = register_netdev(ndev);
	if (status) {
		prerr("Cannot register network device, errno=%d\n", status);
		return status;
	}
	
	//Find and map MAC device
/*	lp->regs = of_iomap(op->dev.of_node, 0);*/	
//	mac_phys_addr = eusrvmem_get_mac_base_addr();
// 	pr("MAC phys : 0x%llX\n", mac_phys_addr);
// 	lp->regs = ioremap(mac_phys_addr, mac_length);
// 	if (!lp->regs)
// 	{
// 		prerr("Cannot not iomap device registers\n");
// 		return 0;
// 	}
// 	dbg1("eth10g_probe(): mapped device registers\n");

	//Find and map DMA device
	//dma_phys_addr = eusrvmem_get_adma_base_addr(); 
	dma_phys_addr = 0x1C100000;
        dma_length = 0x4000;
	dbg1("Got AXI DMA base addr = 0x%llX\n", (uint64_t)dma_phys_addr);
	lp->dma_regs = ioremap(dma_phys_addr, dma_length);
	dbg1("Ioremaped to virtual address 0x%llX\n", (uint64_t)lp->dma_regs);
	if (lp->dma_regs == NULL)
	{
		dev_err(&op->dev, "ERROR: cannot map DMA device\n");
		return -ENODEV;
	}

	//Get MAC address
/*	addr = of_get_property(op->dev.of_node, "local-mac-address", &size);
	if ((!addr) || (size != 6)) {
		prerr("Cannot not find MAC address in device tree\n");
		return -ENODEV;
	}
*/
	memcpy(lp->ndev->dev_addr, default_mac_addr, ndev->addr_len);
	dbg1("eth10g-probe(): configured MAC address.\n");

	
//	platform_set_drvdata(op, ndev);

	//TODO: get IRQ numbers in a proper way
//	lp->tx_irq = eusrvmem_get_admatx_irq();
//	lp->rx_irq = eusrvmem_get_admarx_irq();
	
        lp->tx_irq = 207;
        lp->rx_irq = 215;
        
	if (lp->tx_irq<=0)
	{
		prerr("Cannot get TX IRQ\n");
		return -1;
	}
	if(lp->rx_irq<=0)
	{
		prerr("Cannot get RX IRQ\n");
		return -1;
	}
		
	
	if (lp->tx_irq == lp->rx_irq)
	{
		pr("Same TX/RX IRQ\n");
		lp->irq = lp->tx_irq;
	}else
	{
		pr("Separate TX/RX IRQs\n");
		lp->irq = 0;
	}

	//TODO: what is this?
//	of_node_put(np);
	
	lp->coalesce_count_tx = DMA_DFT_TX_THRESHOLD;
	lp->coalesce_count_rx = DMA_DFT_RX_THRESHOLD;
	
	lp->tx_unaligned32 = 0;
	lp->rx_unaligned32 = 0;
	
	pr("network device initialized\n");

	dbg1("eth10g_probe(): RETURN\n");
	return 0;
}

static int eth10g_remove(struct platform_device *op)
{	
	struct net_device *ndev;
	struct net_private *lp;	
	dbg1("eth10g_remove(): ENTER\n");

	ndev = platform_get_drvdata(op);
	if (ndev==NULL){prerr("ndev is NULL");goto out;}		
	lp = netdev_priv(ndev);	
	if (lp==NULL){prerr("lp is NULL");goto out;}

	//TODO: what does this exactly?
	//netif_carrier_off(ndev);

	if (ndev)
	{
		eth10g_stop(ndev);	
		unregister_netdev(ndev);
	}
	
	dev_set_drvdata(&op->dev, NULL);
	
	if (lp->regs) {iounmap(lp->regs);}	
	if (lp->dma_regs) {iounmap(lp->dma_regs);}
	
	//platform_set_drvdata(op, NULL);
	if (ndev) {free_netdev(ndev);}

	pr("Removed\n");
	dbg1("eth10g_remove(): RETURN\n");
out:	
	return 0;
}


/*===== End of Platform Driver Functions =========*/


MODULE_AUTHOR("John Velegrakis, FORTH-ICS");
MODULE_DESCRIPTION("Euroserver 10Gbps NIC driver for 64bit prototype");
MODULE_LICENSE("GPL");

///@endcond
