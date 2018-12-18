/*  
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <asm/io.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <asm/uaccess.h>

#include <asm/sysreg.h>

#include "kmax_pmu.h"


struct proc_dir_entry *kmax_power;

extern void kaleao_read_pmic(struct seq_file *m);
extern void kaleao_write_pmic(unsigned int i, unsigned int val);


static int kmax_pmu_proc_show(struct seq_file *m, void *v) {
	char *filename = NULL;
	volatile uint32_t *reg;
	void __iomem *reg_raw;
	volatile uint32_t val;
	uint32_t i = 0;

	filename = (char *)((struct file *)m->private)->f_path.dentry->d_iname;

	if(strcmp(filename, "pmu")==0)
	{
		reg = ioremap(PMU_BASE, 0x6200);

		for(i=0; i<PMU_REGS_END; i++)
		{
			val = readl((void *)(((uint64_t)reg) + reg_offsets[i]));
			seq_printf(m, "%s: %X\n", reg_names[i], val);
		}
		iounmap(reg);

	}
	else if (strcmp(filename, "pmu_raw")==0)
	{
		reg_raw = ioremap(PMU_BASE, 0x6200);

                for(i=0x100; i<0x10C; i+=0x4)
                {
                        val = readl(reg_raw + i);
                        seq_printf(m, "PMU[%08X]: 0x%08X\n", i, val);
                }
                iounmap(reg_raw);

		seq_printf(m, "%llX\n", read_sysreg(S3_1_C15_C2_0));
		
	}
	else if(strcmp(filename, "el1_sysreg")==0)
	{
		printk(KERN_INFO "Running on: %d\n", smp_processor_id());		
		seq_printf(m, "%llX\n", read_sysreg(S3_1_C15_C2_0));
	}
	else if(strcmp(filename, "pmic")==0)
	{
		kaleao_read_pmic(m);
	}

	return 0;
}

static ssize_t kmax_pmu_proc_write(struct file *file, const char * buff, size_t size, loff_t * ppos)
{
	char *filename = NULL;
	static char proc_buff[1024];
	uint32_t pos, val;
	uint64_t val2;
	void __iomem *reg_raw;
	int status;

	status = copy_from_user(proc_buff, buff, 1024);
	if(status!=0)
	{
		printk(KERN_INFO "Cannot copy from user!\n");
		return size;
	}

	filename = (char *)file->f_path.dentry->d_iname;
	if(strcmp(filename, "pmic")==0)
	{
		sscanf(proc_buff, "%X %X", &pos, &val);
	
		kaleao_write_pmic(pos, val);
		
	}
	else if(strcmp(filename, "pmu_raw")==0)
	{		
		sscanf(proc_buff, "%X %X", &pos, &val);
		reg_raw = ioremap(PMU_BASE, 0x6200);
		
		writel(val, reg_raw + pos);
				
		iounmap(reg_raw);		
	}
	else if(strcmp(filename, "el1_sysreg")==0)
	{
		
		printk(KERN_INFO "Running on: %d\n", smp_processor_id());		
		sscanf(proc_buff, "%llX", &val2);
		write_sysreg(val2, S3_1_C15_C2_0);
	}

	return size;
}

static int kmax_pmu_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, kmax_pmu_proc_show, (void *) file);
}

struct file_operations kmax_pmu_proc_fops = {
	.owner = THIS_MODULE,
	.open = kmax_pmu_proc_open,
	.read = seq_read,
	.write = kmax_pmu_proc_write,
	.llseek = seq_lseek,
	.release = single_release,
};


static int __init kmax_pmu_init(void)
{	
	kmax_power = proc_mkdir("kmax_power", NULL);
	
	proc_create_data("pmu", 0766, kmax_power, &kmax_pmu_proc_fops, kmax_pmu_proc_show);
	proc_create_data("pmic", 0766, kmax_power, &kmax_pmu_proc_fops, kmax_pmu_proc_show);
	proc_create_data("pmu_raw", 0766, kmax_power, &kmax_pmu_proc_fops, kmax_pmu_proc_show);
	proc_create_data("el1_sysreg", 0766, kmax_power, &kmax_pmu_proc_fops, kmax_pmu_proc_show);
	printk(KERN_INFO "kmax_pmu: Disabling Unused regulators\n");
	
	//BUCK2 rem sense OFF
	kaleao_write_pmic(0x1B, 0xD8);
	//BUCK4 rem sense OFF
	kaleao_write_pmic(0x1F, 0xD8);
	
	//BUCK5 OFF
	//kaleao_write_pmic(0x21, 0x18);
	//BUCK6 OFF
	kaleao_write_pmic(0x23, 0x18);
	//BUCK10 OFF
	kaleao_write_pmic(0x2B, 0x18);
	//LDO10 OFF
	kaleao_write_pmic(0x3D, 0x14);
	//LDO18 OFF
	kaleao_write_pmic(0x45, 0x0);
	//LDO19 OFF
	kaleao_write_pmic(0x46, 0x0);
	//LDO20 OFF
	kaleao_write_pmic(0x47, 0x0);
	//LDO21 OFF
	kaleao_write_pmic(0x48, 0x0);
	//LDO22 OFF
	kaleao_write_pmic(0x49, 0x0);
	//LDO23 OFF
	kaleao_write_pmic(0x4A, 0x0);
	//LDO24 OFF
	kaleao_write_pmic(0x4B, 0x0);
	//LDO25 OFF
	kaleao_write_pmic(0x4C, 0x0);
	//LDO26 OFF
	kaleao_write_pmic(0x4D, 0x0);
	//LDO27 OFF
	kaleao_write_pmic(0x4E, 0x0);

	return 0;
}

static void __exit kmax_pmu_exit(void)
{
	remove_proc_subtree("kmax_power", NULL);
}

module_init(kmax_pmu_init);
module_exit(kmax_pmu_exit);
MODULE_LICENSE("GPL");
