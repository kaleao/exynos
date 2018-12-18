/*
*
* File: $(BOARD)/misc.c
*
*/
#pragma message("*********************************** bootmode.c ***********************************")

#include <common.h>
#include <command.h>
#include <malloc.h>

#include <asm/gpio.h>
#include <asm/arch/power.h>
#include <asm/arch/sysreg.h>

//#define CONFIG_BOOTARGS 	"console=ttySAC3,115200n8 androidboot.console=ttySAC3 no_console_suspend testmode=0 sec_log=512K@0x5FF80000 vmalloc=200M"
#if 0
#define CONFIG_TMD_BOOTARGS 	"console=tty60 no_console_suspend testmode=1"
#define EXYNOS3472_GPIO_BASE    0x11000000
#define GPX0CON_OFFSET  		0x0c00
#define GPX0DAT_OFFSET  		0x0c04
#define GPX0PUD_OFFSET  		0x0c08

#define GPX0CON 		(EXYNOS3472_GPIO_BASE + GPX0CON_OFFSET)
#define GPX0DAT 		(EXYNOS3472_GPIO_BASE + GPX0DAT_OFFSET)
#define GPX0PUD 		(EXYNOS3472_GPIO_BASE + GPX0PUD_OFFSET)
#endif

//int g_update_flag = REBOOTFLAG_NONE;
#define WORD_BANK_ADDR  0xDE000000	/*The font start address in DRAM*/	
int g_factory_mode = 0;
int g_testmode = 0; 
int g_boot_mode = 0;
extern char result_apdebug[10];
unsigned int CpTestMode =0;
unsigned int FacTestMode =0;

//extern unsigned int bootrom_usb_update ;

int enter_mode_a(void);

typedef enum
{
	enter_recovery_mode,
	enter_fastboot_mode,
	enter_eng_mode
} Enter_boot_mode;

/*
extern void LCD_turnon(void);
extern void LCD_turnoff(void);
*/
extern int drv_video_probe(void);
extern void LCD_setfgcolor_font(unsigned int color);
extern void LCD_draw_bgcolor_font();

extern void lcd_draw_string(u32 row, u32 col, char *c, u32 color);
extern void exynos_get_MFP(unsigned char *buf);
//extern int enter_mode_b(Enter_boot_mode bModel);
//extern int check_boot_mode(int mode);

#if 0
static int check_usb(void)
{
        int val;
        int dok;

        /*---gpx0.2 cok ---setting--*/
        val = readl(GPX0CON);
        val &= ~(0xf<<4);
        writel(val,GPX0CON);

        val = readl(GPX0PUD);
        val &= ~(0x3<<2); //cok,pullup/down disable
        writel(val,GPX0PUD);
	
	udelay(100000);

        val = readl(GPX0DAT);
        dok = !!(val & (0x01<<1));
		printf("\n%s:dok = %#x\n",__func__,dok);
        return dok;
}
#endif

#define GPA2CON 0x10580040
#define GPA2DAT 0x10580044
#define GPA2PUD 0x10580048
#define GPA2DRV_SR 0x1058004c

int check_boot_mode(void)
{
    printf("Enter check_boot_mode\n");
    unsigned int reg;
    
    // GPA2_7:Power key
	reg = readl(GPA2CON);     
	reg &= ~(0xf<<28);
	writel(reg, GPA2CON);
	udelay(1);

    reg = 0;
	reg = readl(GPA2PUD);
	reg &= ~(0x3<<14); /*Pull-up/down disabled*/
	//reg |= readl(0x1<<14);
	writel(reg, GPA2PUD);  
	udelay(1);
    
    /*GPA2_0:Volume Up,set to input,pullup*/
	reg = readl(GPA2CON);   
	reg &= ~(0xff);
	writel(reg, GPA2CON);
	udelay(1);

    /*GPA2_1:Volume Down,set to input,pullup*/
	reg = readl(GPA2PUD);
	reg |= (0xf);
	writel(reg, GPA2PUD);  
	udelay(100);
    
    // Read the register value
	reg = readl(GPA2DAT);
   if(reg&0x80){
       printf("[BootMode]Power key is not pressed,reg=%08x\n",reg); 
   }else
      printf("[BootMode]Power key is pressed,reg=%08x\n",reg);

   /*Enter the different mode according to volume + and volume -*/
   if((reg&0x2)==0 && (reg&0x01)!=0){
       /*printf("[p632x]Bootmode: factory mode\n");*/
       printf("[BootMode]Volume Down is pressed\n");
	drv_video_probe();   
       FacTestMode = 1;
   }
   else if((reg&0x01)==0 && (reg&0x2)!=0){
       /*printf("[p632x]Bootmode: cp test mode\n");*/
       printf("[BootMode]Volume Up is pressed\n");
	drv_video_probe();
       CpTestMode = 1;
       LCD_setfgcolor_font(0);
       LCD_draw_bgcolor_font();
       printf("clr screen\n");
       exynos_get_MFP(WORD_BANK_ADDR);
       printf("Bootmode: Uboot Menu\n");
       enter_mode_a();
       
   }
   else if((reg&0x03)==0)
   {
       //exynos_get_MFP(WORD_BANK_ADDR);/*Read font partition to WORD_BANK_ADDR*/
       /*LCD_turnon();
       LCD_setfgcolor(0);
       lcd_draw_bgcolor();
       printf("Bootmode: Uboot Menu\n");
       enter_mode_a();*/
       printf("[BootMode]Volume Up and Down is pressed\n");
	drv_video_probe();
   }
   else{
       /*setenv("bootargs", "");*/
   }

   return 0;


#if 0
	if(!check_usb())//insert USB
	{

		if((reg&0x2)==0 && (reg&0x01)!=0){
			printf("[p632x]Bootmode: factory mode\n");
			FacTestMode = 1;
		}
		else if((reg&0x01)==0 && (reg&0x2)!=0){
			printf("[p632x]Bootmode: cp test mode\n");
			CpTestMode = 1;
		}
		else if((reg&0x03)==0)
		{
			exynos_get_MFP(WORD_BANK_ADDR);	
			LCD_turnon();
			LCD_setfgcolor(0);
			lcd_draw_bgcolor();
			printf("[p632x]Bootmode: fastboot\n");
			enter_mode_b(enter_fastboot_mode);

		}
		else{
			setenv("bootargs", "");
		}
		return 0;
	}
	else//----->Not insert USB
	{
		if((reg&0x2)==0 && (reg&0x01)!=0){
			exynos_get_MFP(WORD_BANK_ADDR);	
			LCD_turnon();
			LCD_setfgcolor(0);
			lcd_draw_bgcolor();
			//	if(!check_usb()){
			//		printf("[p632x]Bootmode: fastboot\n");
			//		enter_mode_b(enter_fastboot_mode);

			printf("[p632x]Bootmode: Uboot Menu\n");
			enter_mode_a();
		}
#ifdef CONFIG_RECOVERY
		else if((reg&0x01)==0 && (reg&0x2)!=0)
		{
			printf("[p632x]Bootmode: recovery\n");
			enter_mode_b(enter_recovery_mode);

		}
#endif
		else if((reg&0x03)==0)
		{
			printf("[p632x]Bootmode: engmode\n");
			enter_mode_b(enter_eng_mode);

		}

		else{
			setenv("bootargs", "");
		}
		return 0;
	}
    
#endif
}

/*void keylight(int on)
{
	__REG(GPX0CON) = (__REG(GPX0CON) & ~(0xf<<24)) | (0x1<<24);
	__REG(GPX0DAT) = (__REG(GPX0DAT) & 0xbf) | (on & 1)<<6;
}*/


#define COLOR_RED 		0xFF0000
#define COLOR_GREEN   	0x00FF00
#define COLOR_BLUE 		0x0000FF

int handle_rf_test(void)
{
	return 0;
}

int handle_sd_update(void)
{
	/*run_command("sdfuse flashwhole", 0);
	run_command("reset", 0);*/
	return 0;
}

int handle_usb_update(void)
{
	run_command("fastboot", 0);
	return 0;
}


#if 1 
#define PARAM_BLKSIZE  	512 /*emmc a block size:512Byte*/
#define debug_offset   0x01f0
int ret;
unsigned long long start, count;
unsigned long mfp_offset;
unsigned char *idbuffer;
static char idflags[70];
#endif

extern int apdebug_changed;
int handle_apdebug_on(void)
{	
	/*set_debug_onoff("apdebug", "on");*/
    apdebug_changed = 1;
    strcpy(result_apdebug,"on");
	return 0;
}

int handle_apdebug_off(void)
{	
	/*set_debug_onoff("apdebug", "off");*/
    apdebug_changed = 1;
    strcpy(result_apdebug,"off");
	return 0;
}

int handle_cpdebug_on(void)
{	
	set_debug_onoff("cpdebug", "on");
	return 0;
}

int handle_cpdebug_off(void)
{	
	set_debug_onoff("cpdebug", "off");
	return 0;
}
extern int set_debug_onoff(char *debug, char *onoff);
int set_debug_onoff(char *debug, char *onoff)
{
#if 0	
	static char run_cmd[100];
	char dest[70];	
	char *name= NULL, *value = NULL, *end= NULL, *mid=NULL;
	int i = 0;
	int place = 0 , n = 0 , j = 0;
	ret = get_raw_area_info("mfp", &start, &count);
	if(ret < 0)
	{
		printf("Fail to get mfp partition\n");
		return 0;
	}
	mfp_offset = start/PARAM_BLKSIZE;
	idbuffer = (unsigned char *) malloc(512);
	memset(idbuffer,'0',512);
	sprintf(run_cmd, "mmc read 0 %lx %lx 1", idbuffer, mfp_offset);
	run_command(run_cmd,0);
	for(i=0;i<100;i++)
	{
		if(idbuffer[i]== '\0')
		{
			idflags[i]=" ";
			idflags[i+1]=idbuffer[i];	
			break;
		}
		idflags[i]=idbuffer[i];
		if (i>=100)
		{
			printf("MFP block id flags too long\n");
			return 0;
		}
	}
	//printf("BUFF:%s\n",idflags);
	name = strstr(idflags, debug);
	if (name == NULL){
		printf("please check the mfp partition!!!\n");
		return 0;
	}
	value = strchr(name, '=');
	if(value == NULL){
		return 0;
	}
	end = strchr(value,' ');
	if(end == NULL)
	{
		return 0;
	}
	//dest = end + 1;
	strcpy(dest, end);
	place = value+1-idflags;
	idflags[place] = '\0';
	strcat(idflags, onoff);
	strcat(idflags, dest);
	//printf("last value:%s\n",idflags);


	sprintf(run_cmd, "mmc write  0 %lx %lx 1", idflags, mfp_offset);
	run_command(run_cmd,0);
	run_command("reset", 0);
#endif
	return 0;
}

int handle_reboot(void)
{
    run_command("reset", 0);
    return 0;
}
int handle_engineering_mode(void)
{
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
	if(om_status == BOOT_UFS){
		setenv("bootcmd", CONFIG_ENGMODE_RESET_BOOTCOMMAND_UFS);
	}
	else {
		setenv("bootcmd", CONFIG_ENGMODE_RESET_BOOTCOMMAND);
	}
	g_testmode =1;
	return 0;
}
int handle_recovery_mode(void)
{
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
	if(om_status == BOOT_UFS){
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND_UFS);
	}
	else {
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}
	return 0;
}
struct TESTARRAY
{
    char name[32];
    int (*handler)(void);
};

struct TESTARRAY MainMenuArry[] = {
	{"RF Test",				handle_rf_test},
	{"SD Update",			handle_sd_update},
	{"USB Update",		    handle_usb_update},
	{"Recovery",			handle_recovery_mode},
	{"AP Debug On",		    handle_apdebug_on},
	{"AP Debug Off",	    handle_apdebug_off},
	{"CP Debug On",		    handle_cpdebug_on},
	{"CP Debug Off",	    handle_cpdebug_off},
	{"Engineering Mode",    handle_engineering_mode},
	{"Reboot",				handle_reboot},	
};

int g_cur_selected = 0;
int g_item_nums = sizeof(MainMenuArry)/sizeof(MainMenuArry[0]);
void draw_item(int index, int color)
{
	char str[4];
	memset(str, 0, 4);

	if (index < 9)
		sprintf(str, "%d.", index+1);
	else
		sprintf(str, "%d", index+1);

	lcd_draw_string(index + 4, 2, str, color);

	lcd_draw_string(index + 4, 4, MainMenuArry[index].name, color);
}

void draw_all_item(int len)
{
	int i = 0;

//	clr_screen();
	for (i = 0; i < len; i++)
	{
		if (i == g_cur_selected)
			draw_item(i, COLOR_RED);
		else
			draw_item(i, COLOR_GREEN);
	}

	return;
}

#define KEY_VOLUMEDOWN		114
#define KEY_VOLUMEUP		115
#define KEY_POWER			116
#define NO_KEY              0
struct namelist
{	
	char name[32];
};
struct namelist menu[] ={	
	" 1.RF Test",	
	" 2.SD Update",	
	" 3.USB Update",	
	" 4.Recovery",	
	" 5.AP Debug on",	
	" 6.AP Debug off",	
	" 7.CP Debug on",	
	" 8.CP Debug off",	
	" 9.Engineering Mode",	
	"10.Reboot"
};
int current_sel=9;

void draw_menu_on_pc(int current_selected)
{
	
	int i;
	if(current_sel != current_selected){
		printf("\033[2J");
		printf("\033[30;32mUBOOT  MENU\033[0m\n");		
		for(i=0;i<10;i++){			
			if(current_selected == i)				
				printf("\033[30;31m>%s \033[0m\n",&menu[i]);			
			else				
				printf("\033[30;32m %s \033[0m\n",&menu[i]);		
		}
		current_sel = current_selected;
	}
}


int enter_mode_a(void)
{
	unsigned int reg_volume , reg_power = 0;
	int key,i,delayflag,key_time,last_key = 0;
	int keyvalue;

	
	lcd_draw_string(2, 1, "UBOOT  MENU", COLOR_GREEN);
	draw_all_item(g_item_nums);
	draw_menu_on_pc(g_cur_selected);
	udelay(1);
	unsigned int temp;
    mdelay(2000);/*delay 2s,give a short time to display menu*/
    delayflag = 1;
	while(1)
	{
		keyvalue = 0;
		/* key detect*/
		reg_volume = readl(GPA2DAT);
		reg_power  = reg_volume;
		if((reg_volume&0x01)==0 && (reg_volume&0x2)!=0){			
			key = KEY_VOLUMEUP;//GPA2_0
		}
		else if((reg_volume&0x2)==0 && (reg_volume&0x01)!=0){				    
			key = KEY_VOLUMEDOWN;//GPA2_1
		} 

		else if((reg_power&0x80)==0x00){		 				  
			key = KEY_POWER;
		} 
		else {
			key = NO_KEY;
		}

		if (tstc()) {
			keyvalue = getc();
			if(keyvalue == 65) 
				key = KEY_VOLUMEUP;
			else if(keyvalue == 66) 
				key = KEY_VOLUMEDOWN;
			else if(keyvalue == 13) 
				key = KEY_POWER;
		}
	    if(delayflag)
		   mdelay(10);
		if(key == last_key ){
			if(key != NO_KEY)
			{
				key_time++;			
				//printf("key_time:%d\n",key_time);
			}
			if(key_time >= 20)
			{
				//printf("key:%d\n",key);
				key_time = 0;
			}
			else
			{
				key = NO_KEY;
			}
		}
		else{
			last_key = key;
			key_time = 0;
		}
		if(key == KEY_VOLUMEUP)
		{
			g_cur_selected -= 1;
			if(g_cur_selected<0)
				g_cur_selected = g_item_nums-1;
		}
		else  if(key == KEY_VOLUMEDOWN)
		{
			g_cur_selected += 1;
			if(g_cur_selected == g_item_nums)
				g_cur_selected = 0;
		}
		else if(key == KEY_POWER)
		{
			if(MainMenuArry[g_cur_selected].handler != NULL)
			{
                LCD_setfgcolor_font(0);
                LCD_draw_bgcolor_font();
		 
				printf("[AVL7420]Choosing: %s\n", MainMenuArry[g_cur_selected].name);
				MainMenuArry[g_cur_selected].handler();
				break;
			}
		}

		draw_menu_on_pc(g_cur_selected);
		draw_all_item(g_item_nums);
	}

	return 0;
}

int enter_mode_b(Enter_boot_mode bModel)
{
 	switch(bModel)
	{

		case enter_recovery_mode:
			MainMenuArry[3].handler();
			break;

		case enter_fastboot_mode:
			lcd_draw_string(9, 6, "FASTBOOT", COLOR_GREEN);
			MainMenuArry[2].handler();
			break;

		case enter_eng_mode:
			MainMenuArry[8].handler();
			break;
	}

	return 0;
}

// end of file
