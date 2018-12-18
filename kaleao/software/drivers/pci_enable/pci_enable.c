/**
 * @file pci_enable.c
 *
 * Simple driver to enable FPGAs PCI express endpoints.
 * 
 * @author Babis Aronis
 * 
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/pm_runtime.h>
#include <linux/msi.h>
#include <linux/pci-aspm.h>
#include <linux/prefetch.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <asm/irq.h>
#include <linux/highmem.h>
#include <linux/slab.h>

#define DEVICE_COUNT_RESOURCE	(6)
#define NETFPGA_REGS_SIZE		256

#define MODULENAME	"kmax_pci"

#define DEBUG
#define VERBOSE



#ifdef DEBUG
///Debug print helper. Activated if DEBUG is defined.
#define dprint(...)	printk(KERN_INFO MODULENAME": DEBUG "__VA_ARGS__);
#else
#define dprint(...)	;
#endif

#ifdef VERBOSE
///Verbose print helper. Activated if VERBOSE is defined.
#define vprint(...)	printk(KERN_INFO MODULENAME": "__VA_ARGS__);
#else
#define vprint(...)	;
#endif

#define errprint(...)	printk(KERN_INFO MODULENAME": ERROR!! "__VA_ARGS__);


int test_irqs = 0;
module_param(test_irqs, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(test_irqs, "0 without test handlers, 1 with test handlers");



///Information about the device

struct netfpga_device {	
	uint64_t phys_addr[DEVICE_COUNT_RESOURCE];	///<Physical addresses for each bar
	uint64_t len[DEVICE_COUNT_RESOURCE];		///<Length of each bar	
};

#define PCI_VENDOR_ID_PLDA	0x1556

///PCI information for the device
static const struct pci_device_id netfpga_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x8024), 0, 0, 0x0 },
	{ PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x7021), 0, 0, 0x0 },
	{ PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x7022), 0, 0, 0x0 },
	{ PCI_DEVICE(PCI_VENDOR_ID_PLDA,   0x1100), 0, 0, 0x0 },	
	{ PCI_DEVICE(PCI_VENDOR_ID_PLDA,   0x11aa), 0, 0, 0x0 },
	{ PCI_DEVICE(PCI_VENDOR_ID_PLDA,  0x1111), 0, 0, 0x0 },
	{0,},
};

struct netfpga_device fpga_dev;

static int netfpga_init_one(struct pci_dev *pdev, const struct pci_device_id *ent);
static void netfpga_remove_one(struct pci_dev *pdev);
static void netfpga_shutdown(struct pci_dev *pdev);

static pci_ers_result_t netfpga_error_detected(struct pci_dev *dev,
		enum pci_channel_state state);


///PCI Error handlers struct
static struct pci_error_handlers netfpga_error_h = {
	.error_detected		= netfpga_error_detected,
};

///PCI Callbacks
static struct pci_driver netfpga_pci_driver = {
	.name		= MODULENAME,
	.id_table	= netfpga_pci_tbl,
	.probe		= netfpga_init_one,
	.remove		= netfpga_remove_one,
	.shutdown	= netfpga_shutdown,
	.err_handler	= &netfpga_error_h,
	//	.driver.pm	= EUSRVMEM_PM_OPS,
};

module_pci_driver(netfpga_pci_driver);




static irqreturn_t test_irq(int irq, void *dev)
{
//	vprint("IRQ[%d] Came!!\n", irq);
	return IRQ_HANDLED;
}






/**
 * (PCIe API) Initiliazes the device. Called when the device is probed.
 * 
 * @param pdev	The pci device
 * @param ent	The device entries
 * 
 * @retval 0		On success
 * @retval error_code	On error
 */
static int netfpga_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{    	
	int status = 0;
	uint32_t i = 0;
	struct msix_entry *msix_entries;

	vprint("Initailizing PCI Device!\n");
	//Enable PCI device
	status = pci_enable_device(pdev);
	if (status < 0)
	{
		errprint("cannot enable PCI device. Exiting.\n");
		goto err_out_free_dev_1;
	}
	vprint("Enabled PCI device\n");

	//Initialize the physical address info table
	for(i=0; i<DEVICE_COUNT_RESOURCE; i++)
	{
		fpga_dev.phys_addr[i] = 0xFFFFFFFFFFFFFFFF;
	}

	//Enable Mem-Wr-Invalidate functionality
	if (pci_set_mwi(pdev) < 0) vprint("WARNING: Mem-Wr-Inval unavailable\n");

	vprint("Discovering PCI BARs...\n");

	//Discover PCI BARs
	for (i=0; i<DEVICE_COUNT_RESOURCE; i++)
	{
		if (!(pci_resource_flags(pdev, i) & IORESOURCE_MEM))
		{
			fpga_dev.phys_addr[i] = 0x0;
			continue;
		}

		//check for weird/broken PCI region reporting
		if (pci_resource_len(pdev, i) < NETFPGA_REGS_SIZE)
		{
			fpga_dev.phys_addr[i] = 0x0;
			continue;
		}
		fpga_dev.len[i] = pci_resource_len(pdev, i);	
		vprint("Found BAR #%d len=%llX\n", i, fpga_dev.len[i]);
	}

	//Request the pci memory regions
	status = pci_request_regions(pdev, MODULENAME);
	if (status < 0) 
	{       
		errprint("cannot claim PCI BARs. Exiting.\n");
		goto err_out_mwi_2;
	}
	vprint("Claimed PCI BARs");

	vprint("Checking found PCI BARs...\n");

	//Check the discovered PCI BARs
	for(i=0; i<DEVICE_COUNT_RESOURCE; i++)
	{		
		if (fpga_dev.phys_addr[i] == 0x0) continue;

		if (!(pci_resource_flags(pdev, i) & IORESOURCE_MEM))
		{
			errprint("BAR #%d not an MMIO resource\n", i);
			fpga_dev.phys_addr[i] = 0x0;
			continue;
		}

		fpga_dev.phys_addr[i] = pci_resource_start(pdev, i);
		if (!fpga_dev.phys_addr[i]) 
		{
			errprint("no I/O resource at PCI BAR #%d, start = %llX\n", i, (u64)fpga_dev.phys_addr[i]);            
			fpga_dev.phys_addr[i] = 0x0;
			continue;
		}   		
		vprint("BAR #%d at 0x%016llX len=0x%016llX is OK.\n", i, fpga_dev.phys_addr[i], fpga_dev.len[i]);		
	}

	//Check if PCI express capability is present
	if (!pci_is_pcie(pdev)) vprint("not PCI Express\n");

	//Set the bus pci master
	pci_set_master(pdev);

#define MSIX_VECTORS	0x8
	//Enable MSI-X
        vprint("Enabling %d MSI-X interrupt vectors\n", MSIX_VECTORS);
        msix_entries = kzalloc(MSIX_VECTORS * sizeof(struct msix_entry), GFP_KERNEL);
        if(msix_entries==0)
        {
                errprint("Cannot Allocate MSI-X Entries structure!\n");
                status = -ENOMEM;
                //goto err_out_free_res_3;
        }
    
        for(i=0; i<MSIX_VECTORS; i++)
        {
                msix_entries[i].entry = i;
        }

	if(0){
		u32 table_offset;
		pci_read_config_dword(pdev, pdev->msix_cap + PCI_MSIX_TABLE, &table_offset);
		vprint("Table Offset: %X\n", table_offset);

	}
    
        status = pci_enable_msix(pdev, msix_entries, MSIX_VECTORS);
        if(status!=0)
	{
                errprint("Could not allocate msi-x interrupt vectors!\n");
 //               goto err_out_free_res_5;
        }

	for(i=0; i<MSIX_VECTORS; i++)
	{
		vprint("IRQ[0]: %d\n", msix_entries[i].vector);		
	}
	
	if(test_irqs==1)
	{
		for(i=0; i<MSIX_VECTORS; i++)
		{
		        vprint("Enabling %d\n", i);
			//Install test_irq handler
			status = request_irq(msix_entries[i].vector, test_irq, IRQF_SHARED, "kmax_test_irq", (void *)(i+1));
		        if(status!=0)
		        {
		                vprint("[ FAILED ]\n");
		        }else
		        {
		                vprint("[  OK  ]\n");
		        }
		}
	}

	vprint("Device Initialization OK!\n");

	return 0;


	//Error cases
	//err_out_release_regions_3:
	pci_release_regions(pdev);
err_out_mwi_2:
	pci_clear_mwi(pdev);
	pci_disable_device(pdev);
err_out_free_dev_1:
	return /*status*/ -1;
}

/**
 * (PCIe API) Releases the board.
 * 
 * @param pdev The pci device struct
 */
static void netfpga_remove_one(struct pci_dev *pdev)
{
	vprint("Releasing resources\n");
	//	if (pci_dev_run_wake(pdev))pm_runtime_get_noresume(&pdev->dev);                


	//Release each BAR fo the device
	pci_release_regions(pdev);

	//Clear Mem-Wr-Inv
	pci_clear_mwi(pdev);

	//Disable whole device
	pci_disable_device(pdev);

	vprint("Disabled PCI device.\n");
}

/**
 * (PCIe API) Shutdown the device.
 * 
 * @param pdev	The pci device struct
 *
 */
static void netfpga_shutdown(struct pci_dev *pdev)
{
	vprint("Shuting down.\n");
}

/**
 * (PCIe API) Error handler callback.
 * 
 * @param dev	The pcie device
 * @param state	The error type
 * 
 * @retval pci_ers_result_t
 */
static pci_ers_result_t netfpga_error_detected(struct pci_dev *dev, enum pci_channel_state state)
{
	errprint("Error Detected!!\n");

	///@todo Replace that with actual error return value.
	return PCI_ERS_RESULT_RECOVERED;
}

MODULE_AUTHOR("Babis Aronis, Kaleao Ltd");
MODULE_DESCRIPTION("Net FPGA PCIe test driver");
MODULE_LICENSE("GPL");
