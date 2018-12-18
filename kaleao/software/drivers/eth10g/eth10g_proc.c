///@cond DEV_DOC

#include "eth10g.h"

#define CHECK_LP {if(proc_lp==NULL){seq_printf(m, "eth10g: WARNING: private network device is NULL\n");return 0;}}

struct net_private *proc_lp;
static char enabled[] = "ON";
static char disabled[] = "OFF";


int create_proc_eth10g(struct net_device *ndev);
int remove_proc_eth10g(struct net_device *dev);
static int proc_open(struct inode *inode, struct  file *file);
static int proc_show(struct seq_file *m, void *v);
static int proc_open_tx_frm_ok(struct inode *inode, struct  file *file);
static int proc_show_tx_frm_ok(struct seq_file *m, void *v);

static int proc_open_rx_stats(struct inode *inode, struct file *file);
static int proc_show_rx_stats(struct seq_file *m, void *v);

static int proc_open_tx_stats(struct inode *inode, struct file *file);
static int proc_show_tx_stats(struct seq_file *m, void *v);

static int proc_open_mac_config(struct inode *inode, struct file *file);
static int proc_show_mac_config(struct seq_file *m, void *v);

static int proc_open_rx_config(struct inode *inode, struct file *file);
static int proc_show_rx_config(struct seq_file *m, void *v);

static int proc_open_tx_config(struct inode *inode, struct file *file);
static int proc_show_tx_config(struct seq_file *m, void *v);

static int proc_open_tx_unaligned(struct inode *inode, struct file *file);
static int proc_show_tx_unaligned(struct seq_file *m, void *v);

static int proc_open_rx_unaligned(struct inode *inode, struct file *file);
static int proc_show_rx_unaligned(struct seq_file *m, void *v);

struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

char *booltostr(uint32_t bool)
{
	if (bool==0){return disabled;}
	return enabled;
}

char *notbooltostr(uint32_t bool)
{
	if (bool==0){return enabled;}
	return disabled;
}

uint64_t nic_reg64(struct net_private *lp, uint32_t lsw_off, uint32_t msw_off)
{
	uint32_t lsw = 0;
	uint64_t msw = 0;
	uint64_t reg = 0;

	pr("nic_reg64():  0x%08X  0x%08X\n", lsw_off, msw_off);
	
	if (lp==0) {return 0;}
	
	lsw = nic_read32(lp, lsw_off);
	msw = (uint64_t)nic_read32(lp, msw_off);
		
	reg = (msw << 32) | lsw;
	return reg;
}

/*===== Setip Functions ==============================*/
int create_proc_eth10g(struct net_device *ndev)
{
	struct net_private *lp = NULL;
	
	lp = netdev_priv(ndev);	
	lp->proc_eth10g = NULL;
	lp->proc_eth10g = (struct proc_eth10g_struct *)kmalloc(sizeof(struct proc_eth10g_struct), GFP_KERNEL);

	if (lp->proc_eth10g==NULL){prerr("not enough mem to create proc fs\n");return 1;}
	proc_lp = lp;
	
	lp->proc_eth10g->dir_main = proc_mkdir("eth10g", 0);
	if (lp->proc_eth10g->dir_main==NULL){prerr("no mem for main dir\n");return 1;}
	
	lp->proc_eth10g->dir_mac = proc_mkdir("mac", lp->proc_eth10g->dir_main);
	if (lp->proc_eth10g->dir_main==NULL){prerr("no mem for mac dir\n");return 1;}

	lp->proc_eth10g->fops_frm_tx_ok = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_frm_tx_ok == NULL){prerr("no mem for fops_frm_tx_ok\n");return 1;}
	lp->proc_eth10g->fops_frm_tx_ok->open = proc_open_tx_frm_ok;
	lp->proc_eth10g->fops_frm_tx_ok->owner = THIS_MODULE;
	lp->proc_eth10g->fops_frm_tx_ok->read = seq_read;
	lp->proc_eth10g->fops_frm_tx_ok->llseek = seq_lseek;
	lp->proc_eth10g->fops_frm_tx_ok->release = single_release;	
	lp->proc_eth10g->frm_tx_ok = proc_create_data("tx_frm_ok", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_frm_tx_ok, proc_show_tx_frm_ok);
	if (lp->proc_eth10g->frm_tx_ok == NULL){prerr("no mem for frm_tx_ok\n");return 1;}		

	lp->proc_eth10g->fops_rx_stats = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_rx_stats == NULL){prerr("no mem for fops_rx_stats\n");return 1;}
	lp->proc_eth10g->fops_rx_stats->open = proc_open_rx_stats;
	lp->proc_eth10g->fops_rx_stats->owner = THIS_MODULE;
	lp->proc_eth10g->fops_rx_stats->read = seq_read;
	lp->proc_eth10g->fops_rx_stats->llseek = seq_lseek;
	lp->proc_eth10g->fops_rx_stats->release = single_release;		
	lp->proc_eth10g->rx_stats = proc_create_data("rx_stats", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_rx_stats, proc_show_rx_stats);
	if (lp->proc_eth10g->rx_stats == NULL){prerr("no mem for rx_stats\n");return 1;}		
	
	lp->proc_eth10g->fops_tx_stats = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_tx_stats == NULL){prerr("no mem for fops_tx_stats\n");return 1;}
	lp->proc_eth10g->fops_tx_stats->open = proc_open_tx_stats;
	lp->proc_eth10g->fops_tx_stats->owner = THIS_MODULE;
	lp->proc_eth10g->fops_tx_stats->read = seq_read;
	lp->proc_eth10g->fops_tx_stats->llseek = seq_lseek;
	lp->proc_eth10g->fops_tx_stats->release = single_release;		
	lp->proc_eth10g->tx_stats = proc_create_data("tx_stats", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_tx_stats, proc_show_tx_stats);
	if (lp->proc_eth10g->tx_stats == NULL){prerr("no mem for tx_stats\n");return 1;}		
	
	lp->proc_eth10g->fops_mac_config = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_mac_config == NULL){prerr("no mem for fops_mac_config\n");return 1;}
	lp->proc_eth10g->fops_mac_config->open = proc_open_mac_config;
	lp->proc_eth10g->fops_mac_config->owner = THIS_MODULE;
	lp->proc_eth10g->fops_mac_config->read = seq_read;
	lp->proc_eth10g->fops_mac_config->llseek = seq_lseek;
	lp->proc_eth10g->fops_mac_config->release = single_release;		
	lp->proc_eth10g->mac_config = proc_create_data("mac_config", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_mac_config, proc_show_mac_config);
	if (lp->proc_eth10g->mac_config == NULL){prerr("no mem for mac_config\n");return 1;}		
	
	lp->proc_eth10g->fops_rx_config = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_rx_config == NULL){prerr("no mem for fops_rx_config\n");return 1;}
	lp->proc_eth10g->fops_rx_config->open = proc_open_rx_config;
	lp->proc_eth10g->fops_rx_config->owner = THIS_MODULE;
	lp->proc_eth10g->fops_rx_config->read = seq_read;
	lp->proc_eth10g->fops_rx_config->llseek = seq_lseek;
	lp->proc_eth10g->fops_rx_config->release = single_release;		
	lp->proc_eth10g->rx_config = proc_create_data("rx_config", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_rx_config, proc_show_rx_config);
	if (lp->proc_eth10g->rx_config == NULL){prerr("no mem for rx_config\n");return 1;}		

	lp->proc_eth10g->fops_tx_config = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_tx_config == NULL){prerr("no mem for fops_tx_config\n");return 1;}
	lp->proc_eth10g->fops_tx_config->open = proc_open_tx_config;
	lp->proc_eth10g->fops_tx_config->owner = THIS_MODULE;
	lp->proc_eth10g->fops_tx_config->read = seq_read;
	lp->proc_eth10g->fops_tx_config->llseek = seq_lseek;
	lp->proc_eth10g->fops_tx_config->release = single_release;		
	lp->proc_eth10g->tx_config = proc_create_data("tx_config", 0644,  lp->proc_eth10g->dir_mac, lp->proc_eth10g->fops_tx_config, proc_show_tx_config);
	if (lp->proc_eth10g->tx_config == NULL){prerr("no mem for tx_config\n");return 1;}		
	lp->proc_eth10g->fops_tx_unaligned = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_tx_unaligned == NULL){prerr("no mem for fops_tx_unaligned\n");return 1;}
	lp->proc_eth10g->fops_tx_unaligned->open = proc_open_tx_unaligned;
	lp->proc_eth10g->fops_tx_unaligned->owner = THIS_MODULE;
	lp->proc_eth10g->fops_tx_unaligned->read = seq_read;
	lp->proc_eth10g->fops_tx_unaligned->llseek = seq_lseek;
	lp->proc_eth10g->fops_tx_unaligned->release = single_release;
	lp->proc_eth10g->tx_unaligned = proc_create_data("tx_unaligned", 0644, lp->proc_eth10g->dir_main, lp->proc_eth10g->fops_tx_unaligned, proc_show_tx_unaligned);
	if (lp->proc_eth10g->tx_unaligned == NULL){prerr("no mem for tx_unaligned\n");return 1;}
	
	lp->proc_eth10g->fops_rx_unaligned = (struct file_operations *)kmalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (lp->proc_eth10g->fops_rx_unaligned == NULL){prerr("no mem for fops_rx_unaligned\n");return 1;}
	lp->proc_eth10g->fops_rx_unaligned->open = proc_open_rx_unaligned;
	lp->proc_eth10g->fops_rx_unaligned->owner = THIS_MODULE;
	lp->proc_eth10g->fops_rx_unaligned->read = seq_read;
	lp->proc_eth10g->fops_rx_unaligned->llseek = seq_lseek;
	lp->proc_eth10g->fops_rx_unaligned->release = single_release;
	lp->proc_eth10g->rx_unaligned = proc_create_data("rx_unaligned", 0644, lp->proc_eth10g->dir_main, lp->proc_eth10g->fops_rx_unaligned, proc_show_rx_unaligned);
	if (lp->proc_eth10g->rx_unaligned == NULL){prerr("no mem for rx_unaligned\n");return 1;}	
	
	
	return 0;
}

int remove_proc_eth10g(struct net_device *dev)
{
	remove_proc_subtree("eth10g", NULL);
	return 0;
}
/*===== End of Setup Functions =======================*/

static int proc_show(struct seq_file *m, void *v)
{
	int (*proc_show_func)(struct seq_file *sf, void *va);
	if (m->private==NULL){seq_printf(m, "eth10g: WARNING: show function is NULL\n");return  0;}
	proc_show_func = m->private;
	return proc_show_func(m, v);
}

static int proc_show_tx_unaligned(struct seq_file *m, void *v)
{
	if (proc_lp==NULL){seq_printf(m, "eth10g: WARNING: private network device is NULL\n");return 0;}
	
#ifdef TX_CHECK_UNALIGNED
	seq_printf(m, "N/A\n");	
	return 0;
#endif
	
	seq_printf(m, "%llu\n", proc_lp->tx_unaligned32);	
	return 0;
}

static int proc_show_rx_unaligned(struct seq_file *m, void *v)
{
	if (proc_lp==NULL){seq_printf(m, "eth10g: WARNING: private network device is NULL\n");return 0;}
	
#ifdef RX_CHECK_UNALIGNED
	seq_printf(m, "N/A\n");	
	return 0;
#endif	
	
	seq_printf(m, "%llu\n", proc_lp->rx_unaligned32);	
	return 0;
}

static int proc_show_tx_frm_ok(struct seq_file *m, void *v) 
{	
	uint32_t lsw = 0;	
	uint64_t msw = 0;
	uint64_t reg = 0;
	
	if (proc_lp==NULL){seq_printf(m, "eth10g: WARNING: private network device is NULL\n");return 0;}
	
	lsw = nic_read32(proc_lp, NIC_STAT_TXOKFLSW_OFFSET);
	msw = (uint64_t)nic_read32(proc_lp, NIC_STAT_TXOKFMSW_OFFSET);	
	reg = (msw << 32) | lsw;  
	
	seq_printf(m, "%llu\n", reg);	
	return 0;
}

static int proc_open_tx_frm_ok(struct inode *inode, struct  file *file) 
{
	return single_open(file, proc_show_tx_frm_ok, NULL);
}

static int proc_show_rx_stats(struct seq_file *m, void *v)
{				
	CHECK_LP;
	seq_printf(m, "RX Statistics\n");
	seq_printf(m, "===== Bytes ===========\n");	
	seq_printf(m, "Bytes          :  %llu\n\n", nic_reg64(proc_lp, NIC_STAT_RXBLSW_OFFSET, NIC_STAT_RXBMSW_OFFSET));	
	seq_printf(m, "===== Frames OK =======\n");	
	seq_printf(m, "OK             :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXOKFLSW_OFFSET, NIC_STAT_RXOKFMSW_OFFSET));
	seq_printf(m, "Broacast       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXBOKFLSW_OFFSET, NIC_STAT_RXBOKFMSW_OFFSET));
	seq_printf(m, "Multicast      :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXMOKFLSW_OFFSET, NIC_STAT_RXMOKFMSW_OFFSET));
	seq_printf(m, "Control        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXCOKFLSW_OFFSET, NIC_STAT_RXCOKFMSW_OFFSET));
	seq_printf(m, "VLAN           :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXVFLSW_OFFSET, NIC_STAT_RXVFMSW_OFFSET));
	seq_printf(m, "Pause          :  %llu\n\n", nic_reg64(proc_lp, NIC_STAT_RXPFLSW_OFFSET, NIC_STAT_RXPFMSW_OFFSET));
		
	seq_printf(m, "===== Frame Sizes =====\n");
	seq_printf(m, "Undersized     :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXUFLSW_OFFSET, NIC_STAT_RXUFMSW_OFFSET));
	seq_printf(m, "Fragmented     :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXFFLSW_OFFSET, NIC_STAT_RXFFMSW_OFFSET));
	seq_printf(m, "64             :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX64LSW_OFFSET, NIC_STAT_RX64MSW_OFFSET));
	seq_printf(m, "64-127         :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX64127LSW_OFFSET, NIC_STAT_RX64127MSW_OFFSET));
	seq_printf(m, "128-255        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX128255LSW_OFFSET, NIC_STAT_RX128255MSW_OFFSET));
	seq_printf(m, "256-511        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX256511LSW_OFFSET, NIC_STAT_RX256511MSW_OFFSET));
	seq_printf(m, "512-1023       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX5121023LSW_OFFSET, NIC_STAT_RX5121023MSW_OFFSET));
	seq_printf(m, "1024-MTU       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RX1024LSW_OFFSET, NIC_STAT_RX1024MSW_OFFSET));
	seq_printf(m, "Oversized      :  %llu\n\n", nic_reg64(proc_lp, NIC_STAT_RXOSLSW_OFFSET, NIC_STAT_RXOSMSW_OFFSET));

	seq_printf(m, "===== Frame Errors ====\n");
	seq_printf(m, "CRC Errors              :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXERRLSW_OFFSET, NIC_STAT_RXERRMSW_OFFSET));	
	seq_printf(m, "Length out of Range     :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXCLOFLSW_OFFSET, NIC_STAT_RXCLOFMSW_OFFSET));		
	seq_printf(m, "CTRL Unsupported Opcode :  %llu\n", nic_reg64(proc_lp, NIC_STAT_RXCFOPLSW_OFFSET, NIC_STAT_RXCFOPMSW_OFFSET));

	
	return 0;
}

static int proc_open_rx_stats(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_rx_stats, NULL);
}

static int proc_show_tx_stats(struct seq_file *m, void *v)
{		
	CHECK_LP;
	seq_printf(m, "TX Statistics\n");
	seq_printf(m, "===== General =========\n");	
	seq_printf(m, "Bytes:         :  %llu\n\n", nic_reg64(proc_lp, NIC_STAT_TXBLSW_OFFSET, NIC_STAT_TXBMSW_OFFSET));	
	seq_printf(m, "===== Frames OK =======\n");	
	seq_printf(m, "OK:            :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXOKFLSW_OFFSET, NIC_STAT_TXOKFMSW_OFFSET));
	seq_printf(m, "Broacast       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXBOKFLSW_OFFSET, NIC_STAT_TXBOKFMSW_OFFSET));
	seq_printf(m, "Multicast      :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXMOKFLSW_OFFSET, NIC_STAT_TXMOKFMSW_OFFSET));
	seq_printf(m, "Control        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXCOKFLSW_OFFSET, NIC_STAT_TXCOKFMSW_OFFSET));
	seq_printf(m, "VLAN           :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXVFLSW_OFFSET, NIC_STAT_TXVFMSW_OFFSET));
	seq_printf(m, "Pause          :  %llu\n\n", nic_reg64(proc_lp, NIC_STAT_TXPFLSW_OFFSET, NIC_STAT_TXPFMSW_OFFSET));	

	seq_printf(m, "===== Frame Sizes =====\n");
	seq_printf(m, "64             :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX64LSW_OFFSET, NIC_STAT_TX64MSW_OFFSET));
	seq_printf(m, "64-127         :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX64127LSW_OFFSET, NIC_STAT_TX64127MSW_OFFSET));
	seq_printf(m, "128-255        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX128255LSW_OFFSET, NIC_STAT_TX128255MSW_OFFSET));
	seq_printf(m, "256-511        :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX256511LSW_OFFSET, NIC_STAT_TX256511MSW_OFFSET));
	seq_printf(m, "512-1023       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX5121023LSW_OFFSET, NIC_STAT_TX5121023MSW_OFFSET));
	seq_printf(m, "1024-MTU       :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TX1024LSW_OFFSET, NIC_STAT_TX1024MSW_OFFSET));
	seq_printf(m, "Oversized      :  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXOSLSW_OFFSET, NIC_STAT_TXOSMSW_OFFSET));	

	seq_printf(m, "===== Errors ==========\n");
	seq_printf(m, "Underrun Errors:  %llu\n", nic_reg64(proc_lp, NIC_STAT_TXMUERRLSW_OFFSET, NIC_STAT_TXMUERRMSW_OFFSET));	

	
	return 0;
}

static int proc_open_tx_stats(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_tx_stats, NULL);
}

static int proc_show_mac_config(struct seq_file *m, void *v)
{
	CHECK_LP;
	seq_printf(m, "MAC Configuration Registers\n");
	seq_printf(m, "RX Config 0              : 0x%X\n", nic_read32(proc_lp, NIC_RCW0_OFFSET));
	seq_printf(m, "RX Config 1              : 0x%X\n", nic_read32(proc_lp, NIC_RCW1_OFFSET));
	seq_printf(m, "TX Config                : 0x%X\n", nic_read32(proc_lp, NIC_TC_OFFSET));
	seq_printf(m, "Flow Control             : 0x%X\n", nic_read32(proc_lp, NIC_FCC_OFFSET));
	seq_printf(m, "Reconciliation Sublayer  : 0x%X\n", nic_read32(proc_lp, NIC_RSC_OFFSET));
	seq_printf(m, "Receiver MTU             : 0x%X\n", nic_read32(proc_lp, NIC_RMCW_OFFSET));
	seq_printf(m, "Transmitter MTU          : 0x%X\n", nic_read32(proc_lp, NIC_TMCW_OFFSET));
	seq_printf(m, "Version                  : 0x%X\n", nic_read32(proc_lp, NIC_V_OFFSET));
	seq_printf(m, "Capability               : 0x%X\n", nic_read32(proc_lp, NIC_C_OFFSET));
	return 0;
}

static int proc_open_tx_unaligned(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_tx_unaligned, NULL);
}

static int proc_open_rx_unaligned(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_rx_unaligned, NULL);
}

static int proc_open_mac_config(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_mac_config, NULL);
}

static int proc_show_rx_config(struct seq_file *m, void *v)
{	
	CHECK_LP;
	seq_printf(m, "===== MAC RX Configuration ============\n");
	seq_printf(m, "Receiver                : %s\n", booltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_RXE_MASK));
	seq_printf(m, "CTRL Frame Length Check : %s\n", notbooltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_CFLCD_MASK));
	seq_printf(m, "Length/Type Error Check : %s\n", notbooltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_LTECD_MASK));
	seq_printf(m, "Preserve Preamble       : %s\n", booltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_RPPE_MASK));
	seq_printf(m, "VLAN                    : %s\n", booltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_VLAN_MASK));	
	seq_printf(m, "In-band FCS             : %s\n", booltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_IBFE_MASK));
	seq_printf(m, "Jumbo Frames            : %s\n", booltostr(nic_read32(proc_lp, NIC_RCW1_OFFSET) & NIC_RCW1_JFE_MASK));	
	return 0;
}

static int proc_open_rx_config(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_rx_config, NULL);
}

static int proc_show_tx_config(struct seq_file *m, void *v)
{	
	CHECK_LP;
	seq_printf(m, "===== MAC TX Configuration ============\n");
	seq_printf(m, "Transmitter           : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_TXE_MASK));
	seq_printf(m, "Preserve Preamble     : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_TPPE_MASK));
	seq_printf(m, "Deficit Idle Count    : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_DICE_MASK));
	seq_printf(m, "Interframe Gap Adjust : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_IGAE_MASK));
	seq_printf(m, "WAN Mode              : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_WANE_MASK));
	seq_printf(m, "VLAN                  : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_VLAN_MASK));	
	seq_printf(m, "In-band FCS           : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_IBFE_MASK));
	seq_printf(m, "Jumbo Frames          : %s\n", booltostr(nic_read32(proc_lp, NIC_TC_OFFSET) & NIC_TC_JFE_MASK));
	return 0;
}

static int proc_open_tx_config(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show_tx_config, NULL);
}

static int proc_open(struct inode *inode, struct  file *file) 
{
	return single_open(file, proc_show, NULL);
}
///@endcond