#include "kgex.h"
#include "kgex-debug.h"
#include "kgex-hw.h"
#include "kgex-pci.h"

#define CHECK_LP                                                               \
{                                                                            \
	if (proc_lp == NULL) {                                                     \
		seq_printf(m, "private network device is NULL\n");                       \
		return 0;                                                                \
	}                                                                          \
}

#define PROC_BUFF_MAX_SIZE 2048
#define PROC_CMD_MAX_SIZE 2

struct kgex_proc_struct *kgex_proc = NULL;

extern struct net_device *net_dev_tbl[KGEX_MAX_NET_DEVS];
extern int tx_bd_num;
extern char *version;
extern char *build_date;

static char *const dma_names[] = {"dma0", "dma1", 0};

static char proc_buff[PROC_BUFF_MAX_SIZE];
static uint32_t proc_buff_size = 0;

static char enabled[] = "ON";
static char disabled[] = "OFF";

static inline void u32_to_ip(char *addr, uint32_t ip) {
	uint32_t ip0, ip1, ip2, ip3;
	ip0 = (ip >> 24) & 0xFF;
	ip1 = (ip >> 16) & 0xFF;
	ip2 = (ip >> 8) & 0xFF;
	ip3 = ip & 0xFF;
	snprintf(addr, 16, "%u.%u.%u.%u", ip0, ip1, ip2, ip3);
}

int kgex_eth_proc_create(uint32_t ethid);

int kgex_proc_create(void);
int kgex_proc_remove(void);

int proc_open_kgex(struct inode *inode, struct file *file);
int proc_show_kgex(struct seq_file *m, void *v);
int proc_open_ctrl(struct inode *inode, struct file *file);
int proc_show_ctrl(struct seq_file *m, void *v);
int proc_open_conf(struct inode *inode, struct file *file);
int proc_show_conf(struct seq_file *m, void *v);
ssize_t proc_write_conf(struct file *file, const char *buff, size_t size,
		loff_t *ppos);
ssize_t proc_write_ctrl(struct file *file, const char *buff, size_t size,
		loff_t *ppos);

static inline uint32_t get_queue_active_len(uint32_t queue_size, uint32_t tail,
		uint32_t curr) {
	if (curr <= tail) {
		return (tail - curr);
	} else if (curr > tail) {
		return (tail + (queue_size - curr));
	}
	return 0;
}

int dma_name_to_id(char *name) {
	uint32_t i = 0;
	if (!name) {
		return -1;
	}

	for (i = 0; i < 2; i++) {
		if (strcmp(name, dma_names[i]) == 0) {
			return i;
		}
	}
	return -1;
}

char *booltostr(uint32_t bool) {
	if (bool == 0) {
		return disabled;
	}
	return enabled;
}

char *notbooltostr(uint32_t bool) {
	if (bool == 0) {
		return enabled;
	}
	return disabled;
}

struct file_operations kgex_fops = {
	.owner = THIS_MODULE,
	.open = proc_open_kgex,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

struct file_operations kgex_conf_fops = {
	.owner = THIS_MODULE,
	.open = proc_open_conf,
	.read = seq_read,
	.write = proc_write_conf,
	.llseek = seq_lseek,
	.release = single_release,
};

struct file_operations kgex_ctrl_fops = {
	.owner = THIS_MODULE,
	.open = proc_open_ctrl,
	.read = seq_read,
	.write = proc_write_ctrl,
	.llseek = seq_lseek,
	.release = single_release,
};

/*===== Setup Functions ==============================*/

int kgex_proc_create(void) {
	uint32_t i = 0;

	kgex_proc = NULL;
	kgex_proc = (struct kgex_proc_struct *)kmalloc(
			sizeof(struct kgex_proc_struct), GFP_KERNEL);
	if (kgex_proc == NULL) {
		prerr("not enough mem to create proc fs\n");
		return 1;
	}

	kgex_proc->dir_main = proc_mkdir("kgex", 0);
	if (kgex_proc->dir_main == NULL) {
		prerr("no mem for main dir\n");
		return 1;
	}

	kgex_proc->version = proc_create_data("version", 0766, kgex_proc->dir_main,
			&kgex_fops, proc_show_kgex);
	if (!kgex_proc->version) {
		prerr("No mem for version proc file for\n");
		return 1;
	}

	kgex_proc->conf_proc.dir = proc_mkdir("conf", kgex_proc->dir_main);
	if (kgex_proc->conf_proc.dir == NULL) {
		prerr("no mem for conf dir\n");
		return 1;
	}

	kgex_proc->conf_proc.mac_mng_addr =
		proc_create_data("mac_mng_addr", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mac_mng_addr) {
		prerr("No mem for mac_mng_addr proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mac_a_addr =
		proc_create_data("mac_a_addr", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mac_a_addr) {
		prerr("No mem for mac_a_addr proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mac_b_addr =
		proc_create_data("mac_b_addr", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mac_b_addr) {
		prerr("No mem for mac_b_addr proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mac_stg_addr =
		proc_create_data("mac_stg_addr", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mac_stg_addr) {
		prerr("No mem for mac_stg_addr proc file for\n");
		return 1;
	}

	kgex_proc->conf_proc.update_srv_ip =
		proc_create_data("update_srv_ip", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.update_srv_ip) {
		prerr("No mem for update_srv_ip proc file for\n");
		return 1;
	}

	kgex_proc->conf_proc.ssd_status =
		proc_create_data("ssd_status", 0766, kgex_proc->conf_proc.dir,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.ssd_status) {
		prerr("No mem for ssd_status proc file for\n");
		return 1;
	}

	kgex_proc->conf_proc.dir_mng = proc_mkdir("mng", kgex_proc->dir_main);
	if (kgex_proc->conf_proc.dir_mng == NULL) {
		prerr("no mem for mng dir\n");
		return 1;
	}

	kgex_proc->conf_proc.mng_command =
		proc_create_data("command", 0766, kgex_proc->conf_proc.dir_mng,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mng_command) {
		prerr("No mem for command proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mng_cmd_resp =
		proc_create_data("cmd_resp", 0766, kgex_proc->conf_proc.dir_mng,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mng_cmd_resp) {
		prerr("No mem for cmd_resp proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mng_status =
		proc_create_data("status", 0766, kgex_proc->conf_proc.dir_mng,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mng_status) {
		prerr("No mem for status proc file for\n");
		return 1;
	}
	kgex_proc->conf_proc.mng_info =
		proc_create_data("info", 0766, kgex_proc->conf_proc.dir_mng,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mng_info) {
		prerr("No mem for info proc file for\n");
		return 1;
	}

	kgex_proc->conf_proc.mng_pci_npuerrs =
		proc_create_data("pci_errs", 0766, kgex_proc->conf_proc.dir_mng,
				&kgex_conf_fops, proc_show_conf);
	if (!kgex_proc->conf_proc.mng_pci_npuerrs) {
		prerr("No mem for info proc file for\n");
		return 1;
	}
	for (i = 0; i < 2; i++) {
		kgex_proc->dma_proc[i].dir = proc_mkdir(dma_names[i], kgex_proc->dir_main);
		if (!kgex_proc->dma_proc[i].dir) {
			prerr("No mem for proc dir %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_dma_mapped_descs = proc_create_data(
				"tx_dma_mapped_descs", 0766, kgex_proc->dma_proc[i].dir,
				&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_dma_mapped_descs) {
			prerr("No mem for tx_dma_mapped_descs proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_dma_mapped_descs = proc_create_data(
				"rx_dma_mapped_descs", 0766, kgex_proc->dma_proc[i].dir,
				&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_dma_mapped_descs) {
			prerr("No mem for rx_dma_mapped_descs proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_ctrl_reg =
			proc_create_data("tx_ctrl_reg", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_ctrl_reg) {
			prerr("No mem for tx_ctrl_reg proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].tx_status_reg =
			proc_create_data("tx_status_reg", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_status_reg) {
			prerr("No mem for tx_status_reg proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_ctrl_reg =
			proc_create_data("rx_ctrl_reg", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_ctrl_reg) {
			prerr("No mem for rx_ctrl_reg proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_status_reg =
			proc_create_data("rx_status_reg", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_status_reg) {
			prerr("No mem for rx_status_reg proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].pci_gen =
			proc_create_data("pci_gen", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].pci_gen) {
			prerr("No mem for pci_gen proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].reviver =
			proc_create_data("reviver", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].reviver) {
			prerr("No mem for reviver proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].reviver_affinity =
			proc_create_data("reviver_affinity", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].reviver_affinity) {
			prerr("No mem for reviver_affinity proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_coalesce =
			proc_create_data("tx_coalesce", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_coalesce) {
			prerr("No meme for tx_coalesce proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_timeout =
			proc_create_data("tx_timeout", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_timeout) {
			prerr("No meme for tx_timeout proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].rx_coalesce =
			proc_create_data("rx_coalesce", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_coalesce) {
			prerr("No meme for rx_coalesce proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].rx_timeout =
			proc_create_data("rx_timeout", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_timeout) {
			prerr("No meme for rx_timeout proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].rx_ip_checksum =
			proc_create_data("rx_ip_checksum", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_ip_checksum) {
			prerr("No meme for rx_ip_checksum proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].regs =
			proc_create_data("regs", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].regs) {
			prerr("No meme for regs proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_dma_errors =
			proc_create_data("tx_dma_errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_dma_errors) {
			prerr("No mem for tx_dma_errors proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_dma_errors =
			proc_create_data("rx_dma_errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_dma_errors) {
			prerr("No mem for rx_dma_errors proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_dma_desc_errors =
			proc_create_data("tx_dma_desc_errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_dma_desc_errors) {
			prerr("No mem for tx_dma_desc_errors proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_dma_desc_errors =
			proc_create_data("rx_dma_desc_errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_dma_desc_errors) {
			prerr("No mem for rx_dma_desc_errors proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_queue_stopped_times = proc_create_data(
				"tx_queue_stopped_times", 0766, kgex_proc->dma_proc[i].dir,
				&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_queue_stopped_times) {
			prerr("No meme for tx_queue_stopped_times proc file for %s\n",
					dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_xmit_times =
			proc_create_data("tx_xmit_times", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_xmit_times) {
			prerr("No meme for tx_xmit_times proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_active_descs =
			proc_create_data("tx_active_descs", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_active_descs) {
			prerr("No meme for tx_active_descs proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_irq_spurious =
			proc_create_data("tx_irq_spurious", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_irq_spurious) {
			prerr("No meme for tx_irq_spurious proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_irq_spurious =
			proc_create_data("rx_irq_spurious", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_irq_spurious) {
			prerr("No meme for rx_irq_spurious proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].nr_dma_errors =
			proc_create_data("nr_dma_errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].nr_dma_errors) {
			prerr("No meme for nr_dma_errors proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].nr_pci_gen_transitions = proc_create_data(
				"nr_pci_gen_transitions", 0766, kgex_proc->dma_proc[i].dir,
				&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].nr_pci_gen_transitions) {
			prerr("No meme for nr_pci_gen_transitions proc file for %s\n",
					dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].nr_pci_retrains =
			proc_create_data("nr_pci_retrains", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].nr_pci_retrains) {
			prerr("No meme for nr_pci_retrains proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].errors =
			proc_create_data("errors", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].errors) {
			prerr("No meme for errors proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].error_print =
			proc_create_data("error_print", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].error_print) {
			prerr("No meme for error_print proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].netif_queue =
			proc_create_data("netif_queue", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].netif_queue) {
			prerr("No meme for netif_queue proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].mem =
			proc_create_data("mem", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].mem) {
			prerr("No meme for mem proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].nr_rc_resets_ok =
			proc_create_data("nr_rc_resets_ok", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].nr_rc_resets_ok) {
			prerr("No meme for nr_rc_resets_ok proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_irq_unknown =
			proc_create_data("tx_irq_unknown", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_irq_unknown) {
			prerr("No meme for tx_irq_unknown proc file for %s\n", dma_names[i]);
			return 1;
		}
		kgex_proc->dma_proc[i].rx_irq_unknown =
			proc_create_data("rx_irq_unknown", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].rx_irq_unknown) {
			prerr("No meme for rx_irq_unknown proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].nr_external_resets =
			proc_create_data("nr_external_resets", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].nr_external_resets) {
			prerr("No meme for nr_external_resets proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].sflr_enable =
			proc_create_data("sflr_enable", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].sflr_enable) {
			prerr("No meme for sflr_enable proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].sflr_frame_size =
			proc_create_data("sflr_frame_size", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].sflr_frame_size) {
			prerr("No meme for sflr_frame_size proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].sflr_irq_delay =
			proc_create_data("sflr_irq_delay", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].sflr_irq_delay) {
			prerr("No meme for sflr_irq_delay proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_xmit_affinity =
			proc_create_data("tx_xmit_affinity", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_xmit_affinity) {
			prerr("No meme for tx_xmit_affinity proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_poll_coalesce =
			proc_create_data("tx_poll_coalesce", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_poll_coalesce) {
			prerr("No meme for tx_poll_coalesce proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_poll_affinity =
			proc_create_data("tx_poll_affinity", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_poll_affinity) {
			prerr("No meme for tx_poll_affinity proc file for %s\n", dma_names[i]);
			return 1;
		}

		kgex_proc->dma_proc[i].tx_poll_jiffies =
			proc_create_data("tx_poll_jiffies", 0766, kgex_proc->dma_proc[i].dir,
					&kgex_ctrl_fops, proc_show_ctrl);
		if (!kgex_proc->dma_proc[i].tx_poll_jiffies) {
			prerr("No meme for tx_poll_jiffies proc file for %s\n", dma_names[i]);
			return 1;
		}
	}

	return 0;
}

int kgex_proc_remove(void) {
	int status = 0;
	uint32_t i = 0;
	if (!kgex_proc) {
		return 0;
	}

	pr("kgex_proc_remove(): remove_proc_subtree()\n");
	remove_proc_subtree("kgex", NULL);
	if (status != 0) {
		prerr("cannot remove proc entries\n");
		return status;
	}

	pr("kgex_proc_remove(): before for()\n");
	for (i = 0; i < 2; i++) {
		if (kgex_proc->dma_proc[i].regs) {
			kfree(kgex_proc->dma_proc[i].regs);
			kgex_proc->dma_proc[i].regs = NULL;
		}
		if (kgex_proc->dma_proc[i].rx_ip_checksum) {
			kfree(kgex_proc->dma_proc[i].rx_ip_checksum);
			kgex_proc->dma_proc[i].rx_ip_checksum = NULL;
		}
		if (kgex_proc->dma_proc[i].rx_timeout) {
			kfree(kgex_proc->dma_proc[i].rx_timeout);
			kgex_proc->dma_proc[i].rx_timeout = NULL;
		}
		if (kgex_proc->dma_proc[i].rx_coalesce) {
			kfree(kgex_proc->dma_proc[i].rx_timeout);
			kgex_proc->dma_proc[i].rx_coalesce = NULL;
		}
		if (kgex_proc->dma_proc[i].tx_timeout) {
			kfree(kgex_proc->dma_proc[i].tx_timeout);
			kgex_proc->dma_proc[i].tx_timeout = NULL;
		}
		if (kgex_proc->dma_proc[i].tx_coalesce) {
			kfree(kgex_proc->dma_proc[i].tx_coalesce);
			kgex_proc->dma_proc[i].tx_coalesce = NULL;
		}
		if (kgex_proc->dma_proc[i].dir) {
			kfree(kgex_proc->dma_proc[i].dir);
			kgex_proc->dma_proc[i].dir = NULL;
		}
	}
	pr("kgex_proc_remove(): removing kgex_proc->dir_main\n");
	if (kgex_proc->dir_main) {
		kfree(kgex_proc->dir_main);
		kgex_proc->dir_main = NULL;
	}
	pr("kgex_proc_remove(): removing kgex_proc\n");
	if (kgex_proc) {
		kfree(kgex_proc);
		kgex_proc = NULL;
	}

	return 0;
}

/*===== End of Setup Functions =======================*/

int proc_show_kgex(struct seq_file *m, void *v) {
	struct file *file = NULL;
	char *filename = NULL;

	if (!m) {
		prerr("proc_show_kgex(): m==NULL\n");
		seq_printf(m, "ERROR: m==NULL\n");
		return 0;
	}

	if (!m->private) {
		seq_printf(m, "ERROR: private_data is NULL\n");
		return 0;
	}
	file = (struct file *)m->private;
	filename = (char *)file->f_path.dentry->d_iname;
	if (!filename) {
		seq_printf(m, "ERROR: filename is NULL\n");
		return 0;
	}

	if (strcmp(filename, "version") == 0) {
		if (!version) {
			prierr("proc_show_kgex(): version==NULL\n");
			return 0;
		}
		if (!build_date) {
			prierr("proc_show_kgex(): build_date==NULL\n");
			return 0;
		}
		seq_printf(m, "%s - %s\n", version, build_date);
		return 0;
	} else {
		seq_printf(m, "ERROR: invalid filename\n");
	}

	return 0;
}

int proc_show_conf(struct seq_file *m, void *v) {
	struct file *file = NULL;
	char *filename = NULL;
	char *parent = NULL;
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	char tmp_str[1024];
	uint32_t val = 0;
	uint32_t id = 0;
	uint32_t lsb = 0;
	uint32_t msb = 0;
	char *lsb_c = NULL;
	char *msb_c = NULL;
	char addr[6];

	if (!m) {
		prerr("proc_show_conf(): m==NULL\n");
		seq_printf(m, "ERROR: m==NULL\n");
		return 0;
	}

	if (!m->private) {
		seq_printf(m, "ERROR: private_data is NULL\n");
		return 0;
	}
	file = (struct file *)m->private;
	filename = (char *)file->f_path.dentry->d_iname;
	if (!filename) {
		seq_printf(m, "ERROR: filename is NULL\n");
		return 0;
	}
	parent = (char *)file->f_path.dentry->d_parent->d_iname;
	if (!parent) {
		seq_printf(m, "ERROR: parent directory filename is NULL\n");
		return 0;
	}

	ndev = net_dev_tbl[0];
	if (!ndev) {
		prerr("PCIe to NPU is not enabled. Doing nothing.\n");
		return 0;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return 0;
	}

	if (strcmp(filename, "update_srv_ip") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_UPDATE_IP_OFF);
		u32_to_ip(tmp_str, val);
		seq_printf(m, "%s", tmp_str);
		return 0;
	}

	if (strcmp(filename, "ssd_status") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_SSD_STATUS_OFF);
		seq_printf(m, "%u", val);
		return 0;
	} else if (strcmp(filename, "mac_a_addr") == 0) {
		id = 0;
	} else if (strcmp(filename, "mac_b_addr") == 0) {
		id = 1;
	} else if (strcmp(filename, "mac_mng_addr") == 0) {
		id = 2;
	} else if (strcmp(filename, "mac_stg_addr") == 0) {
		id = 3;
	} else if (strcmp(filename, "pci_errs") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_PCIERRS_OFF);
		seq_printf(m, "%X", val);
		return 0;
	} else if (strcmp(filename, "command") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_CMD_OFF);
		seq_printf(m, "%X", val);
		return 0;
	} else if (strcmp(filename, "status") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_STAT_OFF);
		seq_printf(m, "%X", val);
		return 0;
	} else if (strcmp(filename, "info") == 0) {
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_INFO_OFF);
		seq_printf(m, "System %s\n",
				(val & 0x80000000) ? "production" : "recovery");
		val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_PROD_RST_OFF);
		seq_printf(m, "%s\n",
				(val & 0x00000001) ? "Production Reset required" : "");
		return 0;
	} else if (strcmp(filename, "cmd_resp") == 0) {
		seq_printf(m, "not implemented yet.\n");
		return 0;
	} else {
		seq_printf(m, "ERROR: invalid filename '%s'\n", filename);
		return 0;
	}

	lsb = gen_io_rd32_prot(lp, lp->configuration_space, id * 2 * 4);
	msb = gen_io_rd32_prot(lp, lp->configuration_space, id * 2 * 4 + 4);

	lsb_c = (char *)&lsb;
	msb_c = (char *)&msb;

	addr[0] = lsb_c[0];
	addr[1] = lsb_c[1];
	addr[2] = lsb_c[2];
	addr[3] = lsb_c[3];
	addr[4] = msb_c[0];
	addr[5] = msb_c[1];

	seq_printf(m, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2],
			addr[3], addr[4], addr[5]);

	return 0;
}

int proc_show_ctrl(struct seq_file *m, void *v) {
	struct file *file = NULL;
	char *filename = NULL;
	char *parent = NULL;
	int id = 0;
	uint32_t value = 0;
	uint64_t tx_total = 0;
	uint64_t rx_total = 0;
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	struct axi_dma_regs *regs = NULL;

	if (!m) {
		prerr("proc_show_ctrl(): m==NULL\n");
		seq_printf(m, "ERROR: m==NULL\n");
		return 0;
	}
	// if (!proc_lp) {seq_printf(m, "ERROR: proc_lp is NULL\n");return 0;}
	if (!m->private) {
		seq_printf(m, "ERROR: private_data is NULL\n");
		return 0;
	}

	file = (struct file *)m->private;
	filename = (char *)file->f_path.dentry->d_iname;
	if (!filename) {
		seq_printf(m, "ERROR: filename is NULL\n");
		return 0;
	}
	parent = (char *)file->f_path.dentry->d_parent->d_iname;
	if (!parent) {
		seq_printf(m, "ERROR: parent directory filename is NULL\n");
		return 0;
	}

	id = dma_name_to_id(parent);
	if (id < 0) {
		seq_printf(m, "ERROR: component not found\n");
		return 0;
	}

	ndev = net_dev_tbl[id];
	if (!ndev) {
		seq_printf(m, "ERROR: ndev==NULL\n");
		return 0;
	}

	lp = netdev_priv(ndev);
	if (!lp) {
		seq_printf(m, "ERROR: lp==NULL\n");
		return 0;
	}

	if (strcmp(filename, "sflr_enable") == 0) {
		value = kgex_sflr_is_enabled(ndev);
		seq_printf(m, "0x%08X\n", value);
		return 0;
	} else if (strcmp(filename, "sflr_frame_size") == 0) {
		value = kgex_sflr_get_frame_size(ndev);
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "sflr_irq_delay") == 0) {
		value = kgex_sflr_get_irq_delay(ndev);
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_xmit_affinity") == 0) {
		value = lp->tx_xmit_affinity;
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_poll_coalesce") == 0) {
		value = lp->tx_poll_coalesce;
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_poll_affinity") == 0) {
		value = lp->tx_poll_affinity;
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_poll_jiffies") == 0) {
		value = lp->tx_poll_jiffies;
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_dma_mapped_descs") == 0) {
		wmb();
		seq_printf(m, "%d\n", lp->tx_dma_mapped_descs);
		return 0;
	} else if (strcmp(filename, "rx_dma_mapped_descs") == 0) {
		wmb();
		seq_printf(m, "%d\n", lp->rx_dma_mapped_descs);
		return 0;
	} else if (strcmp(filename, "reviver_affinity") == 0) {
		seq_printf(m, "%u\n", lp->reviver_affinity);
		return 0;
	} else if (strcmp(filename, "tx_ctrl_reg") == 0) {
		value = dma_read32(lp, DMA_TX_CR_OFFSET);
		seq_printf(m, "0x%08X\n", value);
		return 0;
	} else if (strcmp(filename, "tx_status_reg") == 0) {
		value = dma_read32(lp, DMA_TX_SR_OFFSET);
		seq_printf(m, "0x%08X\n", value);
		return 0;
	} else if (strcmp(filename, "rx_ctrl_reg") == 0) {
		value = dma_read32(lp, DMA_RX_CR_OFFSET);
		seq_printf(m, "0x%08X\n", value);
		return 0;
	} else if (strcmp(filename, "rx_status_reg") == 0) {
		value = dma_read32(lp, DMA_RX_SR_OFFSET);
		seq_printf(m, "0x%08X\n", value);
		return 0;
	} else if (strcmp(filename, "nr_external_resets") == 0) {
		wmb();
		value = lp->nr_external_resets;
	} else if (strcmp(filename, "tx_irq_unknown") == 0) {
		wmb();
		value = lp->tx_irq_unknown;
	} else if (strcmp(filename, "rx_irq_unknown") == 0) {
		wmb();
		value = lp->rx_irq_unknown;
	} else if (strcmp(filename, "nr_rc_resets_ok") == 0) {
		wmb();
		value = lp->nr_rc_resets_ok;
	} else if (strcmp(filename, "mem") == 0) {
		uint32_t i = 0;
		uint32_t val = 0x0;
		for (i = 0; i < 16; i++) {
			val = gen_io_rd32_prot(lp, lp->regs, 0x3000 + i * 4);
			seq_printf(m, "0x%04X: 0x%08X\n", i * 4, val);
		}
		return 0;
	} else if (strcmp(filename, "netif_queue") == 0) {
		wmb();
		if (netif_queue_stopped(ndev)) {
			value = 0;
		} else {
			value = 1;
		}
	} else if (strcmp(filename, "error_print") == 0) {
		wmb();
		value = lp->error_print;
	} else if (strcmp(filename, "errors") == 0) {
		wmb();
		tx_total = lp->tx_dma_errors + lp->tx_dma_desc_errors;

		rx_total = lp->rx_dma_errors + lp->rx_dma_desc_errors;

		seq_printf(m, "-------------------------\n"
				"PCIe Gen2->Gen1  : %llu\n"
				"-------------------------\n"
				"TX               : %u\n"
				"TX Desc          : %u\n"
				"RX               : %u\n"
				"RX Desc          : %u\n"
				"-------------------------\n"
				"TX Total         : %llu\n"
				"RX Total         : %llu\n"
				"-------------------------\n"
				"Total            : %llu\n",
			lp->nr_pci_retrains, lp->tx_dma_errors, lp->tx_dma_desc_errors,
			lp->rx_dma_errors, lp->rx_dma_desc_errors, tx_total, rx_total,
			tx_total + rx_total + lp->nr_pci_retrains);
		return 0;

	} else if (strcmp(filename, "nr_dma_errors") == 0) {
		wmb();
		value = lp->tx_dma_errors + lp->tx_dma_desc_errors + lp->rx_dma_errors +
			lp->rx_dma_desc_errors;
	} else if (strcmp(filename, "nr_pci_gen_transitions") == 0) {
		wmb();
		value = lp->nr_pci_gen_transitions;
	} else if (strcmp(filename, "nr_pci_retrains") == 0) {
		wmb();
		value = lp->nr_pci_retrains;
	} else if (strcmp(filename, "pci_gen") == 0) {
		value = kgex_pci_get_gen(ndev);
	} else if (strcmp(filename, "reviver") == 0) {
		value = lp->revive_jiffies;
	} else if (strcmp(filename, "tx_coalesce") == 0) {
		value = kgex_dma_get_tx_coalesce(ndev);
	} else if (strcmp(filename, "tx_timeout") == 0) {
		value = kgex_dma_get_tx_timeout(ndev);
	} else if (strcmp(filename, "rx_coalesce") == 0) {
		value = kgex_dma_get_rx_coalesce(ndev);
	} else if (strcmp(filename, "rx_timeout") == 0) {
		value = kgex_dma_get_rx_timeout(ndev);
	} else if (strcmp(filename, "rx_ip_checksum") == 0) {
		value = kgex_get_rx_ip_checksum(ndev);

	} else if (strcmp(filename, "regs") == 0) {
		regs = kgex_get_dma_regs(ndev);
		if (!regs) {
			seq_printf(m, "\n");
			return 0;
		}

		seq_printf(m, "TX CR    : 0x%08X\nTX SR    : 0x%08X\nTX_CDESC : "
				"0x%08X\nTX_TDESC : 0x%08X\nRX CR    : 0x%08X\nRX SR    : "
				"0x%08X\nRX_CDESC : 0x%08X\nRX_TDESC : 0x%08X\n",
				regs->tx_cr, regs->tx_sr, regs->tx_cdesc, regs->tx_tdesc,
				regs->rx_cr, regs->rx_sr, regs->rx_cdesc, regs->rx_tdesc);

		kfree(regs);

		return 0;
	} else if (strcmp(filename, "tx_dma_errors") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%u\n", lp->tx_dma_errors);
		return 0;
	} else if (strcmp(filename, "rx_dma_errors") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%u\n", lp->rx_dma_errors);
		return 0;
	} else if (strcmp(filename, "tx_dma_desc_errors") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%u\n", lp->tx_dma_desc_errors);
		return 0;
	} else if (strcmp(filename, "rx_dma_desc_errors") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%u\n", lp->rx_dma_desc_errors);
		return 0;
	} else if (strcmp(filename, "tx_queue_stopped_times") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%llu\n", lp->queue_stopped_times);
		return 0;
	} else if (strcmp(filename, "tx_xmit_times") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%llu\n", lp->tx_xmit_times);
		return 0;
	} else if (strcmp(filename, "tx_active_descs") == 0) {
		value = get_queue_active_len(lp->tx_bd_num, lp->tx_tail, lp->tx_curr);
		seq_printf(m, "%u\n", value);
		return 0;
	} else if (strcmp(filename, "tx_irq_spurious") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%llu\n", lp->tx_irq_spurious);
		return 0;
	} else if (strcmp(filename, "rx_irq_spurious") == 0) {
		wmb();
		rmb();
		seq_printf(m, "%llu\n", lp->rx_irq_spurious);
		return 0;
	} else {
		seq_printf(m, "ERROR: invalid filename\n");
		return 0;
	}

	seq_printf(m, "%u\n", value);

	return 0;
}

int proc_open_kgex(struct inode *inode, struct file *file) {
	if (!inode) {
		prerr("proc_open_kgex(): inode==NULL\n");
	}
	if (!file) {
		prerr("proc_open_kgex(): file==NULL\n");
	}
	return single_open(file, proc_show_kgex, (void *)file);
}

int proc_open_conf(struct inode *inode, struct file *file) {
	if (!inode) {
		prerr("proc_open_conf(): inode==NULL\n");
	}
	if (!file) {
		prerr("proc_open_conf(): file==NULL\n");
	}
	return single_open(file, proc_show_conf, (void *)file);
}

int proc_open_ctrl(struct inode *inode, struct file *file) {
	if (!inode) {
		prerr("proc_open_ctrl(): inode==NULL\n");
	}
	if (!file) {
		prerr("proc_open_ctrl(): file==NULL\n");
	}
	return single_open(file, proc_show_ctrl, (void *)file);
}

ssize_t proc_write_conf(struct file *file, const char *buff, size_t size,
		loff_t *ppos) {
	char *filename = NULL;
	char *parent = NULL;
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	char tmp_str[1024];
	volatile uint32_t val = 0;

	if (size > PROC_BUFF_MAX_SIZE) {
		proc_buff_size = PROC_BUFF_MAX_SIZE;
	} else {
		proc_buff_size = size;
	}

	if (copy_from_user(proc_buff, buff, proc_buff_size)) {
		prerr("proc_write_mac(): Cannot get data from user space.\n");
		return -EFAULT;
	}

	filename = (char *)file->f_path.dentry->d_iname;
	if (!filename) {
		prierr("filename is NULL\n");
		return proc_buff_size;
	}
	parent = (char *)file->f_path.dentry->d_parent->d_iname;
	if (!parent) {
		prierr("parent directory filename is NULL\n");
		return proc_buff_size;
	}

	ndev = net_dev_tbl[0];
	if (!ndev) {
		prerr("PCIe to NPU is not enabled. Doing nothing.\n");
		return proc_buff_size;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return proc_buff_size;
	}

	if (strcmp(filename, "command") == 0) {
		sscanf(proc_buff, "%x", &val);
		gen_io_wr32_prot(lp, lp->configuration_space, CONF_CMD_OFF, val);
	} else if (strcmp(filename, "status") == 0) {
		sscanf(proc_buff, "%x", &val);
		gen_io_wr32_prot(lp, lp->configuration_space, CONF_STAT_OFF, val);
	} else if (strcmp(filename, "info") == 0) {
		sscanf(proc_buff, "%s", tmp_str);
		if (strcmp(tmp_str, "recovery") == 0) {
			val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_INFO_OFF);
			val = val & (~0x80000000);
			gen_io_wr32_prot(lp, lp->configuration_space, CONF_INFO_OFF, val);
		} else if (strcmp(tmp_str, "production") == 0) {
			val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_INFO_OFF);
			val = val | 0x80000000;
			gen_io_wr32_prot(lp, lp->configuration_space, CONF_INFO_OFF, val);
		} else if (strcmp(tmp_str, "updated") == 0) {
			val = gen_io_rd32_prot(lp, lp->configuration_space, CONF_PROD_RST_OFF);
			val = val & ~0x1;
			gen_io_wr32_prot(lp, lp->configuration_space, CONF_PROD_RST_OFF, val);
		}
	} else {
		prierr("unknown filename '%s'\n", filename)
	}

	return proc_buff_size;
}

ssize_t proc_write_ctrl(struct file *file, const char *buff, size_t size,
		loff_t *ppos) {
	char *filename = NULL;
	char *parent = NULL;
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	int value = 0;
	int id = 0;
	int status = 0;
	uint32_t offt = 0x0;
	uint32_t val = 0x0;

	if (size > PROC_BUFF_MAX_SIZE) {
		proc_buff_size = PROC_BUFF_MAX_SIZE;
	} else {
		proc_buff_size = size;
	}

	if (copy_from_user(proc_buff, buff, proc_buff_size)) {
		prerr("proc_write_mac(): Cannot get data from user space.\n");
		return -EFAULT;
	}

	filename = (char *)file->f_path.dentry->d_iname;
	if (!filename) {
		prierr("filename is NULL\n");
		return proc_buff_size;
	}
	parent = (char *)file->f_path.dentry->d_parent->d_iname;
	if (!parent) {
		prierr("parent directory filename is NULL\n");
		return proc_buff_size;
	}

	if (strcmp(filename, "mem") == 0) {
		sscanf(proc_buff, "%X %X", &offt, &val);

		id = dma_name_to_id(parent);
		if (id < 0) {
			prerr("dma not found\n");
			return proc_buff_size;
		}

		ndev = net_dev_tbl[id];
		if (!ndev) {
			prerr("ndev==NULL\n");
			return 0;
		}
		lp = netdev_priv(ndev);

		pr("Writing to offeset 0x%08X = 0x%08X\n", offt, val);
		gen_io_wr32_prot(lp, lp->regs, offt, val);
		return proc_buff_size;
	}

	sscanf(proc_buff, "%d", &value);

	id = dma_name_to_id(parent);
	if (id < 0) {
		prerr("dma not found\n");
		return proc_buff_size;
	}

	ndev = net_dev_tbl[id];
	if (!ndev) {
		prerr("ndev==NULL\n");
		return 0;
	}
	lp = netdev_priv(ndev);

	if (strcmp(filename, "sflr_enable") == 0) {
		if (value == 0) {
			pr("Disabling SFLR...\n");
			kgex_sflr_disable(ndev);
		} else {
			pr("Enabling SFLR...\n");
			kgex_sflr_enable(ndev);
		}
	} else if (strcmp(filename, "sflr_frame_size") == 0) {
		pr("Setting SFLR Frame Size threshold to %u Bytes \n", value);
		kgex_sflr_set_frame_size(ndev, value);
	} else if (strcmp(filename, "sflr_irq_delay") == 0) {
		pr("Setting SFLR IRQ delay time to %u clock cycles \n", value);
		kgex_sflr_set_irq_delay(ndev, value);
	} else if (strcmp(filename, "tx_poll_coalesce") == 0) {
		pr("Setting TX poll coalescing value to %u frames\n", value);
		lp->tx_poll_coalesce = value;
		wmb();
	} else if (strcmp(filename, "tx_poll_affinity") == 0) {
		pr("Setting TX poll affinity to CPU core %u\n", value);
		lp->tx_poll_affinity = value;
		wmb();
	} else if (strcmp(filename, "tx_poll_jiffies") == 0) {
		pr("Setting TX poll jiffies to %u\n", value);
		lp->tx_poll_jiffies = value;
		wmb();
	} else if (strcmp(filename, "reviver_affinity") == 0) {
		lp->reviver_affinity = value;
		wmb();
	} else if (strcmp(filename, "tx_ctrl_reg") == 0) {
		dma_write32(lp, DMA_TX_CR_OFFSET, value);
	} else if (strcmp(filename, "tx_status_reg") == 0) {
		dma_write32(lp, DMA_TX_SR_OFFSET, value);
	} else if (strcmp(filename, "rx_ctrl_reg") == 0) {
		dma_write32(lp, DMA_RX_CR_OFFSET, value);
	} else if (strcmp(filename, "rx_status_reg") == 0) {
		dma_write32(lp, DMA_RX_SR_OFFSET, value);
	} else if (strcmp(filename, "error_print") == 0) {
		wmb();
		if (value) {
			lp->error_print = 1;
		} else {
			lp->error_print = 0;
		}
	} else if (strcmp(filename, "pci_gen") == 0) {
		status = kgex_pci_set_target_gen(ndev, value);
		if (status != 0) {
			return proc_buff_size;
		}
		kgex_pci_retrain(ndev);
	} else if (strcmp(filename, "reviver") == 0) {
		if (lp->reviver_work) {
			if (lp->revive_jiffies == 0) {
				if (value < 10 && value != 0) {
					lp->revive_jiffies = 10;
				} else {
					lp->revive_jiffies = value;
				}
				if (lp->irqs == 0) {
					pr("set reviver jiffies to %d, will be activatproc_opened when %s "
							"goes up.\n",
							lp->revive_jiffies, ndev->name);
				} else {
					schedule_delayed_work(&(lp->reviver_work->work), lp->revive_jiffies);
					pr("started reviver with %d jiffies for %s\n", lp->revive_jiffies,
							ndev->name);
				}
			} else {
				if (value < 10 && value != 0) {
					lp->revive_jiffies = 10;
				} else {
					lp->revive_jiffies = value;
				}
				pr("set reviver schedule jiffies to %d for %s\n", value, ndev->name);
			}
		} else {
			pr("reviver work is not enabled, doing nothing.\n");
		}
	} else if (strcmp(filename, "tx_coalesce") == 0) {
		lp->coalesce_count_tx = value;
		kgex_dma_set_tx_coalesce(ndev, value);
	} else if (strcmp(filename, "tx_timeout") == 0) {
		lp->timeout_tx = value;
		kgex_dma_set_tx_timeout(ndev, value);
	} else if (strcmp(filename, "rx_coalesce") == 0) {
		lp->coalesce_count_rx = value;
		kgex_dma_set_rx_coalesce(ndev, value);
	} else if (strcmp(filename, "rx_timeout") == 0) {
		lp->timeout_rx = value;
		kgex_dma_set_rx_timeout(ndev, value);
	} else if (strcmp(filename, "rx_ip_checksum") == 0) {
		kgex_set_rx_ip_checksum(ndev, value);
	} else {
		prerr("invalid filename\n");
		return 0;
	}

	return proc_buff_size;
}
