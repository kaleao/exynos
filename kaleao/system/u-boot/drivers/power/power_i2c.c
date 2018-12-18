#include <common.h>
#include <config.h>
#include <asm/io.h>

// CHG_SDA - GPD2_0
// CHG_SCL - GPD2_1
#define GPD2CON (0x134700E0)
#define GPD2DAT (0x134700E4)
#define GPD2PUD (0x134700E8)
#define GPD2DRV (0x134700EC)
#define GPD2CONPDN (0x134700F0)
#define GPD2PUDPDN (0x134700F4)

static void mydelay(unsigned long val) {
	udelay(val);
}

typedef enum {
	INPUT = 0,
	OUTPUT
} io_dir;

#define exynos_iic_scl_set(val) \
	writel((readl(GPD2DAT)&(~(1<<1)))|((val?1:0)<<1), GPD2DAT)

#define exynos_iic_sda_set(val) \
	writel((readl(GPD2DAT)&(~(1<<0)))|((val?1:0)<<0), GPD2DAT)

#define exynos_iic_sda_get(val) \
	((readl(GPD2DAT))&(0x1<<0))
	
static void exynos_iic_sda_as(io_dir d)
{
	writel((readl(GPD2CON)&(~(0xF<<0)))|((d&(0x1))<<0), GPD2CON);
}

void exynos_iic_init(void)
{
	exynos_iic_sda_set(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	writel((readl(GPD2CON)&(~(0xFF<<0)))|(0x11<<0), GPD2CON);
	writel(readl(GPD2PUD)&(~(0xF<<0)), GPD2PUD); // Disable Pull-Up/Down
	mydelay(1000);
}

static void exynos_iic_start(void)
{
	exynos_iic_sda_set(1);
	exynos_iic_scl_set(1);
	mydelay(1);	 
	exynos_iic_sda_set(0);
	mydelay(1);
	mydelay(1);
	exynos_iic_scl_set(0);
	mydelay(1);
}

static void i2c_repstart(void)
{
	exynos_iic_scl_set(0);
	
	exynos_iic_sda_set(1);
	mydelay(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	mydelay(1);
	exynos_iic_sda_set(0);
	mydelay(1);
	mydelay(1);
	exynos_iic_scl_set(0);
	mydelay(1);
}

static void exynos_iic_stop(void)
{
	exynos_iic_scl_set(0);

	exynos_iic_sda_set(0);
	mydelay(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	mydelay(1);
	exynos_iic_sda_set(1);
	mydelay(1);
	mydelay(1);
}

static void exynos_iic_send_ack(void)
{
	exynos_iic_sda_set(0);
	mydelay(1);
	exynos_iic_scl_set(0);
	mydelay(1);
	mydelay(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	exynos_iic_scl_set(0);
}

static void exynos_iic_send_nack(void)
{
	exynos_iic_sda_set(1);
	mydelay(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	mydelay(1);
	exynos_iic_scl_set(0);
	mydelay(1);
}

static int exynos_iic_rcv_ack(void)
{
	int i, ret=0;

	exynos_iic_sda_as(INPUT);
	mydelay(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	mydelay(1);
	for (i=0; i<5; i++)
	{
		if(!exynos_iic_sda_get())
		{
			ret = 1;
			//printf("!exynos_iic_sda_get():%d\n",ret);
			break;
		}
	}
	exynos_iic_scl_set(0);
	mydelay(1);
	exynos_iic_sda_set(1);
	exynos_iic_sda_as(OUTPUT);

	return ret;
}

static int exynos_iic_send_byte(unsigned char val)
{
	int i;

	exynos_iic_scl_set(0);

	for (i=0; i<8; i++)
	{
		exynos_iic_sda_set((val&(0x80>>i))?1:0);
		mydelay(1);
		exynos_iic_scl_set(1);
		mydelay(1);
		mydelay(1);
		exynos_iic_scl_set(0);
		mydelay(1);
	}
	
	return exynos_iic_rcv_ack();
}

static int exynos_iic_send_word(unsigned int val)
{
	int i;

	exynos_iic_scl_set(0);

	for (i=0; i<16; i++)
	{
		exynos_iic_sda_set((val&(0x8000>>i))?1:0);
		//printf("sda: %x\n",(val&(0x8000>>i))?1:0);
		mydelay(1);
		exynos_iic_scl_set(1);
		mydelay(1);
		mydelay(1);
		exynos_iic_scl_set(0);
		mydelay(1);
	}
	
	return exynos_iic_rcv_ack();
}

static unsigned char exynos_iic_rcv_byte(void)
{
	int i;
	unsigned char rdata = 0;

	mydelay(1);
	exynos_iic_sda_as(INPUT);
	for(i=0; i<8; i++)
	{
		exynos_iic_scl_set(1);
		mydelay(1);
		mydelay(1);
		rdata |= ((exynos_iic_sda_get()>>0x0)<<(7-i));
		//printf("rcv byte:%x\n",rdata);
		//printf("rcv byte:%x\n",exynos_iic_sda_get()>> 0x0);
		exynos_iic_scl_set(0);
		mydelay(1);
		if (i<7) mydelay(1);
	}
	exynos_iic_sda_as(OUTPUT);

	return rdata;
}

static unsigned int exynos_iic_rcv_word(void)
{
	int i;
	unsigned int rdata = 0;

	mydelay(1);
	exynos_iic_sda_as(INPUT);
	for(i=0; i<8; i++)
	{
		exynos_iic_scl_set(1);
		mydelay(1);
		mydelay(1);
		//rdata |= ((exynos_iic_sda_get()>> 0x0)<<(7-i));
		//printf("rcv byte:%x\n",exynos_iic_sda_get()>> 0x0);
		exynos_iic_scl_set(0);
		mydelay(1);
		if (i<8) mydelay(1);
	}
	exynos_iic_rcv_ack();

	for(i=0; i<8; i++)
	{
		exynos_iic_scl_set(1);
		mydelay(1);
		mydelay(1);
		rdata |= ((exynos_iic_sda_get()>> 0x0)<<(15-i));
		//printf("rcv byte:%x\n",exynos_iic_sda_get()>> 0x0);
		exynos_iic_scl_set(0);
		mydelay(1);
		if (i<8) mydelay(1);
	}
	
	exynos_iic_sda_as(OUTPUT);

	return rdata;
}

int exynos_iic_write(unsigned char chip, unsigned char reg, unsigned int val)
{
	unsigned char ac = 0;
again:
	exynos_iic_start();
	if(!exynos_iic_send_byte(chip)){
		printf("No ACK -1-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	if(!exynos_iic_send_byte(reg)){
		printf("No ACK -2-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	if(!exynos_iic_send_byte(val&0xFF)){
		printf("No ACK -3-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	if(!exynos_iic_send_byte((val>>8)&0xFF)){
		printf("No ACK -3-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	exynos_iic_stop();

	return 0;
}

int exynos_iic_read(unsigned char chip, unsigned char reg)
{
	unsigned int val1=0,val2=0, ac = 0;
again:
	mydelay(1);
	exynos_iic_start();
	if(!exynos_iic_send_byte(chip)) {
		printf("No ACK -1-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	if(!exynos_iic_send_byte(reg)) {
		printf("No ACK -2-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	i2c_repstart();

	if(!exynos_iic_send_byte(chip+1)) {
		printf("No ACK -3-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	val1 =exynos_iic_rcv_byte();
	exynos_iic_send_ack();

	val2 =exynos_iic_rcv_byte();
	exynos_iic_send_nack();

	exynos_iic_stop();
	//printf("val1=%x,val2=%x\n",val1,val2);
	//return (int)(val2<<8|val1);
	return (int)(val1<<8|val2);	//according to chip

}

int exynos_iic_read_byte(unsigned char chip, unsigned char reg)
{
	unsigned int val1=0, ac = 0;
again:
	mydelay(1);
	exynos_iic_start();
	if(!exynos_iic_send_byte(chip)) {
		printf("No ACK -1-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	if(!exynos_iic_send_byte(reg)) {
		printf("No ACK -2-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	i2c_repstart();
	
	if(!exynos_iic_send_byte(chip+1)) {
		printf("No ACK -3-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}

	val1 =exynos_iic_rcv_byte();

	exynos_iic_send_nack();

	exynos_iic_stop();

	return val1;

}

int exynos_iic_write_byte(unsigned char chip, unsigned char reg, unsigned int val)
{
	unsigned char ac = 0;
again:
	exynos_iic_start();
	if(!exynos_iic_send_byte(chip)){
		printf("No ACK -1-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	if(!exynos_iic_send_byte(reg)){
		printf("No ACK -2-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	if(!exynos_iic_send_byte(val&0xFF)){
		printf("No ACK -3-\n");
		exynos_iic_stop();
		ac++;
		if (ac>3) return -1;
		mydelay(20);
		goto again;
	}
	
	exynos_iic_stop();

	return 0;
}

// i2c addr (7bit)
// cw2015 - 0x62
// bq24297 - 0x6b
#define CW2015_I2C_ADDR (0x62<<1)
#define BQ24297_I2C_ADDR (0x6b<<1)

// SYSTEM_STATUS_REG 0x08
// VENDOR_PART_REV_STATUS_REG 0x0A
int bq24297_ReadReg(unsigned char reg)
{
	int val;
	unsigned char chip;
	chip = BQ24297_I2C_ADDR;
	val = exynos_iic_read_byte(chip, reg);
	printf("%s, reg[%d]: %#x\n", __func__, reg, val);
	return val;
}

// VCELL_MSB 0x02
// SOC_MSB 0x04
int cw2015_ReadReg(unsigned char reg)
{
	int val;
	unsigned char chip;
	chip = CW2015_I2C_ADDR;
	val = exynos_iic_read_byte(chip, reg);
	printf("%s, reg[%d]: %#x\n", __func__, reg, val);
	return val;
}

