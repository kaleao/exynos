/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ARCH_GPIO_H
#define __ASM_ARCH_GPIO_H

#ifndef __ASSEMBLY__
struct s5p_gpio_bank {
	unsigned int	con;
	unsigned int	dat;
	unsigned int	pull;
	unsigned int	drv;
	unsigned int	pdn_con;
	unsigned int	pdn_pull;
	unsigned char	res1[8];
};

struct exynos4_gpio_part1 {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank e2;
	struct s5p_gpio_bank e3;
	struct s5p_gpio_bank e4;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
};

struct exynos4_gpio_part2 {
	struct s5p_gpio_bank j0;
	struct s5p_gpio_bank j1;
	struct s5p_gpio_bank k0;
	struct s5p_gpio_bank k1;
	struct s5p_gpio_bank k2;
	struct s5p_gpio_bank k3;
	struct s5p_gpio_bank l0;
	struct s5p_gpio_bank l1;
	struct s5p_gpio_bank l2;
	struct s5p_gpio_bank y0;
	struct s5p_gpio_bank y1;
	struct s5p_gpio_bank y2;
	struct s5p_gpio_bank y3;
	struct s5p_gpio_bank y4;
	struct s5p_gpio_bank y5;
	struct s5p_gpio_bank y6;
	struct s5p_gpio_bank res1[80];
	struct s5p_gpio_bank x0;
	struct s5p_gpio_bank x1;
	struct s5p_gpio_bank x2;
	struct s5p_gpio_bank x3;
};

struct exynos4_gpio_part3 {
	struct s5p_gpio_bank z;
};

struct exynos5_gpio_part1 {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank b1;
	struct s5p_gpio_bank b2;
	struct s5p_gpio_bank b3;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank y0;
	struct s5p_gpio_bank y1;
	struct s5p_gpio_bank y2;
	struct s5p_gpio_bank y3;
	struct s5p_gpio_bank y4;
	struct s5p_gpio_bank y5;
	struct s5p_gpio_bank y6;
	struct s5p_gpio_bank res1[0x3];
	struct s5p_gpio_bank c4;
	struct s5p_gpio_bank res2[0x48];
	struct s5p_gpio_bank x0;
	struct s5p_gpio_bank x1;
	struct s5p_gpio_bank x2;
	struct s5p_gpio_bank x3;
};

struct exynos5_gpio_part2 {
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank h1;
};

struct exynos5_gpio_part3 {
	struct s5p_gpio_bank v0;
	struct s5p_gpio_bank v1;
	struct s5p_gpio_bank v2;
	struct s5p_gpio_bank v3;
	struct s5p_gpio_bank res1[0x20];
	struct s5p_gpio_bank v4;
};

struct exynos5_gpio_part4 {
	struct s5p_gpio_bank z;
};

struct exynos3_gpio_part1 {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank b;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
};
struct exynos3_gpio_part2 {
	struct s5p_gpio_bank k0;
	struct s5p_gpio_bank k1;
	struct s5p_gpio_bank k2;
	struct s5p_gpio_bank res1;//In fact MMC3 channel didn't use
	struct s5p_gpio_bank l0;
	struct s5p_gpio_bank res2[2];
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank res3;
	struct s5p_gpio_bank e2;
	struct s5p_gpio_bank res4[0x6];
	struct s5p_gpio_bank M0;
	struct s5p_gpio_bank M1;
	struct s5p_gpio_bank M2;
	struct s5p_gpio_bank M3;
	struct s5p_gpio_bank M4;
	struct s5p_gpio_bank res5[0x48];
	struct s5p_gpio_bank x0;
	struct s5p_gpio_bank x1;
	struct s5p_gpio_bank x2;
	struct s5p_gpio_bank x3;
};

struct exynos5412_gpio_part2 {
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank c4;
	struct s5p_gpio_bank d1;
};

struct exynos5412_gpio_part3 {
	struct s5p_gpio_bank h0;
	struct s5p_gpio_bank y7;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
};

struct exynos5412_gpio_part4 {
	struct s5p_gpio_bank y0;
	struct s5p_gpio_bank y1;
	struct s5p_gpio_bank y5;
	struct s5p_gpio_bank y6;
	struct s5p_gpio_bank y2;
	struct s5p_gpio_bank y3;
	struct s5p_gpio_bank y4;
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank b1;
	struct s5p_gpio_bank b2;
	struct s5p_gpio_bank b3;
	struct s5p_gpio_bank b4;
};

struct exynos5260_gpio_part1 {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank b1;
	struct s5p_gpio_bank b2;
	struct s5p_gpio_bank b3;
	struct s5p_gpio_bank b4;
	struct s5p_gpio_bank b5;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank d2;
	struct s5p_gpio_bank e0;
	struct s5p_gpio_bank e1;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank k0;
	unsigned char res1[0x9e0];
	struct s5p_gpio_bank x0;
	struct s5p_gpio_bank x1;
	struct s5p_gpio_bank x2;
	struct s5p_gpio_bank x3;
};

struct exynos5260_gpio_part2 {
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank c4;
};

struct exynos5260_gpio_part3 {
	struct s5p_gpio_bank z0;
	struct s5p_gpio_bank z1;
};

/* exynos5430 */
struct exynos5430_gpio_alive {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank a3;
};

struct exynos5430_gpio_aud {
	struct s5p_gpio_bank z0;
	struct s5p_gpio_bank z1;
};

struct exynos5430_gpio_cpif {
	struct s5p_gpio_bank v6;
};

struct exynos5430_gpio_fsys {
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank r4;
	struct s5p_gpio_bank r0;
	struct s5p_gpio_bank r1;
	struct s5p_gpio_bank r2;
	struct s5p_gpio_bank r3;
};

struct exynos5430_gpio_nfc {
	struct s5p_gpio_bank j0;
};

struct exynos5430_gpio_peric {
	struct s5p_gpio_bank v7;
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank d2;
	struct s5p_gpio_bank d4;
	struct s5p_gpio_bank d5;
	struct s5p_gpio_bank d8;
	struct s5p_gpio_bank d6;
	struct s5p_gpio_bank d7;
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank f4;
	struct s5p_gpio_bank f5;
	unsigned char res1[0x3c];
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank g3;
};

struct exynos5430_gpio_touch {
	struct s5p_gpio_bank j1;
};

struct exynos7420_gpio_alive {
	struct s5p_gpio_bank a0;
	struct s5p_gpio_bank a1;
	struct s5p_gpio_bank a2;
	struct s5p_gpio_bank a3;
};

struct exynos7420_gpio_bus0 {
	struct s5p_gpio_bank b0;
	struct s5p_gpio_bank c0;
	struct s5p_gpio_bank c1;
	struct s5p_gpio_bank c2;
	struct s5p_gpio_bank c3;
	struct s5p_gpio_bank d0;
	struct s5p_gpio_bank d1;
	struct s5p_gpio_bank d2;
	struct s5p_gpio_bank d4;
	struct s5p_gpio_bank d5;
	struct s5p_gpio_bank d6;
	struct s5p_gpio_bank d7;
	struct s5p_gpio_bank d8;
	struct s5p_gpio_bank g0;
	struct s5p_gpio_bank g3;
};

struct exynos7420_gpio_bus1 {
	unsigned char res1[0x20];
	struct s5p_gpio_bank f0;
	struct s5p_gpio_bank f1;
	struct s5p_gpio_bank f2;
	struct s5p_gpio_bank f3;
	struct s5p_gpio_bank f4;
	struct s5p_gpio_bank f5;
	struct s5p_gpio_bank g1;
	struct s5p_gpio_bank g2;
	struct s5p_gpio_bank h1;
	struct s5p_gpio_bank v6;
};

struct exynos7420_gpio_fsys0 {
	struct s5p_gpio_bank r4;
};

struct exynos7420_gpio_fsys1 {
	struct s5p_gpio_bank r0;
	struct s5p_gpio_bank r1;
	struct s5p_gpio_bank r2;
	struct s5p_gpio_bank r3;
};

struct exynos7580_gpio_alive {
        struct s5p_gpio_bank a0;
        struct s5p_gpio_bank a1;
        struct s5p_gpio_bank a2;
        struct s5p_gpio_bank a3;
};

struct exynos7580_gpio_ese {
        struct s5p_gpio_bank c0;
};

struct exynos7580_gpio_fsys {
        struct s5p_gpio_bank r0;
        struct s5p_gpio_bank r1;
        struct s5p_gpio_bank r2;
        struct s5p_gpio_bank r3;
        struct s5p_gpio_bank r4;
};

struct exynos7580_gpio_mif {
        struct s5p_gpio_bank m0;
};

struct exynos7580_gpio_top {
        struct s5p_gpio_bank b0;
        struct s5p_gpio_bank b1;
        struct s5p_gpio_bank b2;
        struct s5p_gpio_bank c1;
        struct s5p_gpio_bank c2;
        struct s5p_gpio_bank c5;
        struct s5p_gpio_bank d0;
        struct s5p_gpio_bank d1;
        struct s5p_gpio_bank d2;
        struct s5p_gpio_bank d3;
        struct s5p_gpio_bank e0;
        struct s5p_gpio_bank e1;
        struct s5p_gpio_bank f0;
        struct s5p_gpio_bank f1;
        struct s5p_gpio_bank f2;
};

/* functions */
void s5p_gpio_cfg_pin(struct s5p_gpio_bank *bank, int gpio, int cfg);
void s5p_gpio_direction_output(struct s5p_gpio_bank *bank, int gpio, int en);
void s5p_gpio_direction_input(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_value(struct s5p_gpio_bank *bank, int gpio, int en);
unsigned int s5p_gpio_get_value(struct s5p_gpio_bank *bank, int gpio);
void s5p_gpio_set_pull(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_drv(struct s5p_gpio_bank *bank, int gpio, int mode);
void s5p_gpio_set_rate(struct s5p_gpio_bank *bank, int gpio, int mode);

/* GPIO pins per bank  */
#define GPIO_PER_BANK 8

#define exynos4_gpio_part1_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos4_gpio_part1 *) \
			       EXYNOS4_GPIO_PART1_BASE)->bank)) \
	    - EXYNOS4_GPIO_PART1_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define EXYNOS4_GPIO_PART1_MAX ((sizeof(struct exynos4_gpio_part1) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define exynos4_gpio_part2_get_nr(bank, pin) \
	(((((((unsigned int) &(((struct exynos4_gpio_part2 *) \
				EXYNOS4_GPIO_PART2_BASE)->bank)) \
	    - EXYNOS4_GPIO_PART2_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin) + EXYNOS4_GPIO_PART1_MAX)

#if defined(CONFIG_CPU_EXYNOS5260)
#define exynos5_gpio_part1_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5260_gpio_part1 *) \
			       EXYNOS5260_GPIO_PART1_BASE)->bank)) \
	    - EXYNOS5260_GPIO_PART1_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define EXYNOS5_GPIO_PART1_MAX ((sizeof(struct exynos5260_gpio_part1) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define exynos5_gpio_part2_get_nr(bank, pin) \
	(((((((unsigned int) &(((struct exynos5260_gpio_part2 *) \
				EXYNOS5260_GPIO_PART2_BASE)->bank)) \
	    - EXYNOS5260_GPIO_PART2_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin) + EXYNOS5260_GPIO_PART1_MAX)

#define EXYNOS5_GPIO_PART2_MAX ((sizeof(struct exynos5_gpio_part2) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define exynos5_gpio_part3_get_nr(bank, pin) \
	(((((((unsigned int) &(((struct exynos5260_gpio_part3 *) \
				EXYNOS5260_GPIO_PART3_BASE)->bank)) \
	    - EXYNOS5260_GPIO_PART3_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin) + EXYNOS5260_GPIO_PART2_MAX)

#elif defined(CONFIG_CPU_EXYNOS5430)
#define exynos5_gpio_alive_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_alive *) \
			       EXYNOS5430_GPIO_ALIVE_BASE)->bank)) \
	    - EXYNOS5430_GPIO_ALIVE_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_aud_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_aud *) \
			       EXYNOS5430_GPIO_AUD_BASE)->bank)) \
	    - EXYNOS5430_GPIO_AUD_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_cpif_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_cpif *) \
			       EXYNOS5430_GPIO_CPIF_BASE)->bank)) \
	    - EXYNOS5430_GPIO_CPIF_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_fsys_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_fsys *) \
			       EXYNOS5430_GPIO_FSYS_BASE)->bank)) \
	    - EXYNOS5430_GPIO_FSYS_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_nfc_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_nfc *) \
			       EXYNOS5430_GPIO_NFC_BASE)->bank)) \
	    - EXYNOS5430_GPIO_NFC_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_peric_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_peric *) \
			       EXYNOS5430_GPIO_PERIC_BASE)->bank)) \
	    - EXYNOS5430_GPIO_PERIC_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define exynos5_gpio_touch_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5430_gpio_touch *) \
			       EXYNOS5430_GPIO_TOUCH_BASE)->bank)) \
	    - EXYNOS5430_GPIO_TOUCH_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define EXYNOS5430_GPIO_PERIC_MAX ((sizeof(struct exynos5430_gpio_peric) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define EXYNOS5430_GPIO_FSYS_MAX ((sizeof(struct exynos5430_gpio_fsys) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)
#elif defined(CONFIG_CPU_EXYNOS7420)
#define exynos5_gpio_alive_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos7420_gpio_alive *) \
			       EXYNOS7420_GPIO_ALIVE_BASE)->bank)) \
	    - EXYNOS7420_GPIO_ALIVE_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)
#define EXYNOS7420_GPIO_BUS0_MAX ((sizeof(struct exynos7420_gpio_bus0) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)
#define EXYNOS7420_GPIO_FSYS0_MAX ((sizeof(struct exynos7420_gpio_fsys0) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)
#define EXYNOS7420_GPIO_FSYS1_MAX ((sizeof(struct exynos7420_gpio_fsys1) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)
#else /* CONFIG_CPU_EXYNOS5430 */
#define exynos5_gpio_part1_get_nr(bank, pin) \
	((((((unsigned int) &(((struct exynos5_gpio_part1 *) \
			       EXYNOS5_GPIO_PART1_BASE)->bank)) \
	    - EXYNOS5_GPIO_PART1_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin)

#define EXYNOS5_GPIO_PART1_MAX ((sizeof(struct exynos5_gpio_part1) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define exynos5_gpio_part2_get_nr(bank, pin) \
	(((((((unsigned int) &(((struct exynos5_gpio_part2 *) \
				EXYNOS5_GPIO_PART2_BASE)->bank)) \
	    - EXYNOS5_GPIO_PART2_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin) + EXYNOS5_GPIO_PART1_MAX)

#define EXYNOS5_GPIO_PART2_MAX ((sizeof(struct exynos5_gpio_part2) \
			    / sizeof(struct s5p_gpio_bank)) * GPIO_PER_BANK)

#define exynos5_gpio_part3_get_nr(bank, pin) \
	(((((((unsigned int) &(((struct exynos5_gpio_part3 *) \
				EXYNOS5_GPIO_PART3_BASE)->bank)) \
	    - EXYNOS5_GPIO_PART3_BASE) / sizeof(struct s5p_gpio_bank)) \
	  * GPIO_PER_BANK) + pin) + EXYNOS5_GPIO_PART2_MAX)
#endif /* CONFIG_CPU_EXYNOS5260 */

static inline unsigned int s5p_gpio_base(int nr)
{
	if (cpu_is_exynos7()) {
#if defined(CONFIG_CPU_EXYNOS7420)
		if (nr < EXYNOS7420_GPIO_FSYS0_MAX)
			return EXYNOS7420_GPIO_FSYS0_BASE;
		else if (nr < EXYNOS7420_GPIO_FSYS1_MAX)
			return EXYNOS7420_GPIO_FSYS1_BASE;
		else
			return EXYNOS7420_GPIO_BUS0_BASE;
#endif
	} else if (cpu_is_exynos5()) {
#if defined(CONFIG_CPU_EXYNOS5430)
		if (nr < EXYNOS5430_GPIO_PERIC_MAX)
			return EXYNOS5430_GPIO_PERIC_BASE;
		else if (nr < EXYNOS5430_GPIO_FSYS_MAX)
			return EXYNOS5430_GPIO_FSYS_BASE;
		else
			return EXYNOS5430_GPIO_ALIVE_BASE;
#elif defined(CONFIG_CPU_EXYNOS5260)
		if (nr < EXYNOS5_GPIO_PART1_MAX)
			return EXYNOS5260_GPIO_PART1_BASE;
		else if (nr < EXYNOS5_GPIO_PART2_MAX)
			return EXYNOS5260_GPIO_PART2_BASE;
		else
			return EXYNOS5260_GPIO_PART3_BASE;
#else
#if 0
		if (nr < EXYNOS5_GPIO_PART1_MAX)
			return EXYNOS5_GPIO_PART1_BASE;
		else if (nr < EXYNOS5_GPIO_PART2_MAX)
			return EXYNOS5_GPIO_PART2_BASE;
		else
			return EXYNOS5_GPIO_PART3_BASE;
#endif
#endif /* CONFIG_CPU_EXYNOS5260 */
	} else if (cpu_is_exynos4()) {
		if (nr < EXYNOS4_GPIO_PART1_MAX)
			return EXYNOS4_GPIO_PART1_BASE;
		else
			return EXYNOS4_GPIO_PART2_BASE;
	}

	return 0;
}

#endif

/* Pin configurations */
#define GPIO_INPUT	0x0
#define GPIO_OUTPUT	0x1
#define GPIO_IRQ	0xf
#define GPIO_FUNC(x)	(x)

/* Pull mode */
#define GPIO_PULL_NONE	0x0
#define GPIO_PULL_DOWN	0x1
#define GPIO_PULL_UP	0x3

/* Drive Strength level */
#define GPIO_DRV_1X	0x0
#define GPIO_DRV_3X	0x1
#define GPIO_DRV_2X	0x2
#define GPIO_DRV_4X	0x3
#define GPIO_DRV_FAST	0x0
#define GPIO_DRV_SLOW	0x1
#endif
