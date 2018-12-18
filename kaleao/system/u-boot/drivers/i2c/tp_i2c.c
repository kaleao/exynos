#include <common.h>
#include <config.h>
#include <asm/io.h>

// CHG_SDA - GPD2_0
// CHG_SCL - GPD2_1
#define GPJ1CON (0x14CE0000)
#define GPJ1DAT (0x14CE0004)
#define GPJ1PUD (0x14CE0008)
#define GPJ1DRV (0x14CE000C)
#define GPJ1CONPDN (0x14CE0010)
#define GPJ1PUDPDN (0x14CE0014)

static void mydelay(unsigned long val) {
	udelay(val);
}

typedef enum {
	INPUT = 0,
	OUTPUT
} io_dir;

#define exynos_iic_scl_set(val) \
	writel((readl(GPJ1DAT)&(~(1<<1)))|((val?1:0)<<1), GPJ1DAT)

#define exynos_iic_sda_set(val) \
	writel((readl(GPJ1DAT)&(~(1<<0)))|((val?1:0)<<0), GPJ1DAT)

#define exynos_iic_sda_get(val) \
	((readl(GPJ1DAT))&(0x1<<0))
	
static void exynos_iic_sda_as(io_dir d)
{
	writel((readl(GPJ1CON)&(~(0xF<<0)))|((d&(0x1))<<0), GPJ1CON);
}

void exynos_iic_tp_init(void)
{
	exynos_iic_sda_set(1);
	exynos_iic_scl_set(1);
	mydelay(1);
	writel((readl(GPJ1CON)&(~(0xFF<<0)))|(0x11<<0), GPJ1CON);
	writel(readl(GPJ1PUD)&(~(0xF<<0)), GPJ1PUD); // Disable Pull-Up/Down
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

static int exynos_iic_write(unsigned char chip, unsigned char reg, unsigned int val)
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

static int exynos_iic_read(unsigned char chip, unsigned char reg)
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

static int exynos_iic_read_byte(unsigned char chip, unsigned char reg)
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

static int exynos_iic_write_byte(unsigned char chip, unsigned char reg, unsigned int val)
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
#define MELFAS_I2C_ADDR (0x48<<1)
#define ATMEL_I2C_ADDR (0x4a<<1)
#define SYNAPTICSDSX_I2C_ADDR (0x20<<1)

int atmel_ReadReg(unsigned char reg)
{
	int val;
	unsigned char chip;
	chip = ATMEL_I2C_ADDR;
	val = exynos_iic_read_byte(chip, reg);
	printf("%s, reg[%d]: %#x\n", __func__, reg, val);
	return val;
}

int melfas_ReadReg(unsigned char reg)
{
	int val;
	unsigned char chip;
	chip = MELFAS_I2C_ADDR;
	val = exynos_iic_read_byte(chip, reg);
	printf("%s, reg[%d]: %#x\n", __func__, reg, val);
	return val;
}

int synaptics_ReadReg(unsigned char reg)
{
	int val;
	unsigned char chip;
	chip = SYNAPTICSDSX_I2C_ADDR;
	val = exynos_iic_read_byte(chip, reg);
	printf("%s, reg[%d]: %#x\n", __func__, reg, val);
	return val;
}


