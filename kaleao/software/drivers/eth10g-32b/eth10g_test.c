
#include <linux/module.h>  
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>  
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/platform_device.h>

#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/if_vlan.h>

#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>	

#define DRIVER              "eth10gtest"
#define DRIVER_NAME	    "eth10gtest"
#define DRIVER_DESCRIPTION  "Kmax 10Gbps AXI DMA registers test driver"
#define DRIVER_VERSION      "1.00"

static struct of_device_id eth10gtest_match[] = {
	{ .compatible = "kmax,kmax-eth10g-1.00.a", },		
	{},	
};

MODULE_DEVICE_TABLE(of, eth10gtest_match);

int eth10gtest_init (struct net_device **dev_out);
static int eth10gtest_probe(struct platform_device *op);
static int eth10gtest_remove(struct platform_device *op);

static struct platform_driver eth10gtest_driver = {
	.probe		= eth10gtest_probe,
	.remove		= eth10gtest_remove,	
	.driver = {
		.name	= "eth10gtest",
		.owner	= THIS_MODULE,
		.of_match_table = eth10gtest_match,
	},
};

module_platform_driver(eth10gtest_driver);

#define pr(fmt, ...) {pr_info(DRIVER_NAME ": " fmt, ##__VA_ARGS__);}
#define prerr(fmt, ...) {pr_info(DRIVER_NAME ": ERROR: " fmt, ##__VA_ARGS__);}
#define prwarn(fmt, ...) {pr_info(DRIVER_NAME ": WARNING: " fmt, ##__VA_ARGS__);}

#define ETH10G_DEBUG_LEVEL_1

#ifdef ETH10G_DEBUG_LEVEL_3
	#ifndef ETH10G_DEBUG_LEVEL_2
		#define ETH10G_DEBUG_LEVEL_2
	#endif
	#define dbg3(fmt, ...) {pr_info(fmt, ##__VA_ARGS__);}
#else
	#define dbg3(fmt, ...) {;}
#endif


#ifdef ETH10G_DEBUG_LEVEL_2
	#ifndef ETH10G_DEBUG_LEVEL_1
		#define ETH10G_DEBUG_LEVEL_1
	#endif
	#define dbg2(fmt, ...) {pr_info(fmt, ##__VA_ARGS__);}
#else
	#define dbg2(fmt, ...) {;}
#endif


#ifdef ETH10G_DEBUG_LEVEL_1
	#define dbg1(fmt, ...) {pr_info(fmt, ##__VA_ARGS__);}
#else
	#define dbg1(fmt, ...) {;}
#endif 

#define hw_write32(base, reg, val)					\
	__raw_writel((val), (void __iomem *)((uint64_t)(base) + (uint64_t)(reg)))
	
#define hw_write64(base, reg, val)					\
	__raw_writeq((val), (void __iomem *)((uint64_t)(base) + (uint64_t)(reg)))	

#define ReadReg32(BASE_ADDRESS, RegOffset) \
          *(unsigned int *)(BASE_ADDRESS + RegOffset);
          
#define ReadReg64(BASE_ADDRESS, RegOffset) \
          *(uint64_t *)(BASE_ADDRESS + RegOffset);  

#define LO32(val)	((uint32_t)((val) & 0xFFFFFFFF))
#define HI32(val)	((uint32_t)((val) >> 32))
#define WHOLE(lo, hi)	((uint64_t) (hi) << 32 | (lo)); 

    //Write sequence 08 0 10
    #define DMA_TX_CR_OFFSET	0x00000000 /* Channel control */
    #define DMA_TX_SR_OFFSET	0x00000004 /* Status */
    #define DMA_TX_CDESC_OFFSET	0x00000008 /* Current descriptor pointer */
    #define DMA_TX_TDESC_OFFSET	0x00000010 /* Tail descriptor pointer */

    //Write sequence 38 30 40
    #define DMA_RX_CR_OFFSET	0x00000030 /* Channel control */
    #define DMA_RX_SR_OFFSET	0x00000034 /* Status */
    #define DMA_RX_CDESC_OFFSET	0x00000038 /* Current descriptor pointer */
    #define DMA_RX_TDESC_OFFSET	0x00000040 /* Tail descriptor pointer */

    static inline void dma_write32(void *addr, uint32_t offset, uint32_t val)
    {    
                    hw_write32(addr, offset, val);		
    }

    static inline uint32_t dma_read32(void *addr, uint32_t offset)
    {    
                    return ReadReg32(addr, offset);
    }
    
static void __iomem *mem = NULL;    


int eth10gtest_init (struct net_device **dev_out)
{
	pr("init\n");	
	return 0;
}

static int eth10gtest_probe(struct platform_device *op)
{		
        void __iomem *dma_regs = NULL;
	phys_addr_t dma_phys_addr;
	phys_addr_t dma_length;        
        uint32_t val = 0xEDB23000;
        uint32_t *base = NULL;
        uint32_t cr = 0;
        
        dma_addr_t temp = 0x00000000EDB24000;
                
	dbg1("eth10gtest_probe(): ENTER\n");	
        
        dma_phys_addr = 0x1C100000 + 0x4000;
	dma_length = 0x1000;		
        
 	dma_regs = ioremap(dma_phys_addr, dma_length);        
	if (dma_regs == NULL)
	{
		dev_err(&op->dev, "ERROR: cannot map DMA device\n");
		return -ENODEV;
	}
	dbg1("eth10gtest_probe(): ioremapped lp->dma_regs\n");
        mem = dma_regs;
        
        
        cr = ReadReg32(dma_regs, DMA_RX_CR_OFFSET);	

        if(HI32(temp)!=0) {prwarn("eth10gtest_probe(): HI32(temp)");}       
                
                
        dbg1("eth10gtest_probe(): writind dma DMA_TX_CDESC_OFFSET val=0x%016llX LO32(temp)=0x%08X\n", temp, LO32(temp));        
        hw_write32(dma_regs, DMA_TX_CDESC_OFFSET, LO32(temp));        
        wmb();
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", ioread32((void __iomem *)((uint64_t)(dma_regs) + DMA_TX_CDESC_OFFSET)));
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", dma_read32(dma_regs, DMA_TX_CDESC_OFFSET));
        
        
        dbg1("eth10gtest_probe(): writind dma DMA_TX_CDESC_OFFSET val=0x%08X with hw_write32()\n", val);
        hw_write32(dma_regs, DMA_TX_CDESC_OFFSET, val);
        wmb();                
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", ioread32((void __iomem *)((uint64_t)(dma_regs) + DMA_TX_CDESC_OFFSET)));
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", dma_read32(dma_regs, DMA_TX_CDESC_OFFSET));
        
        dbg1("eth10gtest_probe(): writind dma DMA_TX_CDESC_OFFSET val=0x%08X with base[2] = val\n", val);
        base = (uint32_t *)dma_regs;
        base[2] = val;
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", ioread32((void __iomem *)((uint64_t)(dma_regs) + DMA_TX_CDESC_OFFSET)));
        dbg1("eth10gtest_probe(): DMA_TX_CDESC_OFFSET = 0x%08X\n", dma_read32(dma_regs, DMA_TX_CDESC_OFFSET));
        

	dbg1("eth10gtest_probe(): RETURN\n");
	return 0;
}



static int eth10gtest_remove(struct platform_device *op)
{        
	dbg1("eth10gtest_remove(): ENTER\n");	            
        if (mem) {iounmap(mem);}
        dbg1("eth10gtest_remove(): RETURN\n");	            
	return 0;
}


MODULE_AUTHOR("John Velegrakis, Kaleao Ltd.");
MODULE_DESCRIPTION("Kmax 10Gbps AXI DMA registers test driver");
MODULE_LICENSE("GPL");