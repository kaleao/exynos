/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

#ifndef _VAL_SMDK3472_H
#define _VAL_SMDK3472_H

#include <config.h>
#include <version.h>

#if defined (CONFIG_SYS_FIN_26)
#define FIN 	26000000
#endif
#if defined (CONFIG_SYS_FIN_24)
#define FIN 	24000000
#endif

#define CONFIG_CLK_ARM_700
#define CONFIG_CLK_BPLL_800
#define CONFIG_CLK_MPLL_1600
#define CONFIG_CLK_EPLL_200
#define CONFIG_CLK_VPLL_266
#define CONFIG_CLK_UPLL_50

/*
** APLL/MPLL/BPLL setting
*/
#define PLL_LOCKTIME	0x1C20
/* APLL_LOCK		*/
#define APLL_LOCK_VAL	(PLL_LOCKTIME)
/* MPLL_LOCK	*/
#define MPLL_LOCK_VAL	(PLL_LOCKTIME)
/* BPLL_LOCK	*/
#define BPLL_LOCK_VAL	(PLL_LOCKTIME)
/* EPLL_LOCK		*/
#define EPLL_LOCK_VAL	(PLL_LOCKTIME)
/* VPLL_LOCK		*/
#define VPLL_LOCK_VAL	(PLL_LOCKTIME)
/* UPLL_LOCK		*/
#define UPLL_LOCK_VAL	(PLL_LOCKTIME)


#if defined(CONFIG_CLK_ARM_800)
#if defined (CONFIG_SYS_FIN_26)
#define APLL_MDIV	0x320
#define APLL_PDIV	0xD
#define APLL_SDIV	0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define APLL_MDIV	0xC8
#define APLL_PDIV	0x3
#define APLL_SDIV	0x1
#endif
#endif


#if defined(CONFIG_CLK_ARM_700)
#if defined (CONFIG_SYS_FIN_26)
#define APLL_MDIV	0x2BC
#define APLL_PDIV	0xD
#define APLL_SDIV	0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define APLL_MDIV	0xAF
#define APLL_PDIV	0x3
#define APLL_SDIV	0x1
#endif
#endif

#if defined(CONFIG_CLK_ARM_1000)
#if defined (CONFIG_SYS_FIN_26)
#define APLL_MDIV	0x3E8
#define APLL_PDIV		0xD
#define APLL_SDIV		0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define APLL_MDIV	0xFA
#define APLL_PDIV		0x3
#define APLL_SDIV		0x1
#endif
#endif

#if defined(CONFIG_CLK_BPLL_800)
#if defined (CONFIG_SYS_FIN_26)
#define BPLL_MDIV	0x320
#define BPLL_PDIV	0xD
#define BPLL_SDIV	0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define BPLL_MDIV	0xC8
#define BPLL_PDIV	0x3
#define BPLL_SDIV	0x1
#endif
#endif


#if defined(CONFIG_CLK_BPLL_1066)
#if defined (CONFIG_SYS_FIN_26)
#define BPLL_MDIV	0x148
#define BPLL_PDIV		0x4
#define BPLL_SDIV		0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define BPLL_MDIV	0x215
#define BPLL_PDIV		0x6
#define BPLL_SDIV		0x1
#endif
#endif

#if defined(CONFIG_CLK_BPLL_1333)
#if defined (CONFIG_SYS_FIN_26)
#define BPLL_MDIV	0x29B
#define BPLL_PDIV		0xD
#define BPLL_SDIV		0x0
#endif
#if defined (CONFIG_SYS_FIN_24)
#define BPLL_MDIV	0x29B
#define BPLL_PDIV		0x6
#define BPLL_SDIV		0x1
#endif
#endif


#if defined(CONFIG_CLK_MPLL_1600)
#if defined (CONFIG_SYS_FIN_26)
#define MPLL_MDIV	0x320
#define MPLL_PDIV	0xD
#define MPLL_SDIV	0x0
#endif
#if defined (CONFIG_SYS_FIN_24)
#define MPLL_MDIV	0xC8
#define MPLL_PDIV	0x3
#define MPLL_SDIV	0x0
#endif
#endif

#if defined(CONFIG_CLK_MPLL_800)
#if defined (CONFIG_SYS_FIN_24)
#define MPLL_MDIV	0xC8
#define MPLL_PDIV	0x3
#define MPLL_SDIV	0x1
#endif
#endif

#if defined(CONFIG_CLK_UPLL_50)
#if defined (CONFIG_SYS_FIN_26)
#define UPLL_MDIV	0x320
#define UPLL_PDIV	0xD
#define UPLL_SDIV	0x5
#endif
#if defined (CONFIG_SYS_FIN_24)
#define UPLL_MDIV	0xC8
#define UPLL_PDIV	0x3
#define UPLL_SDIV	0x5
#endif
#endif


/* APLL_CON1	*/
#define APLL_CON1_VAL	0x00A0FC00		//reset value; AFC_ENB =0; AFC = 0x0; LOCK_EN =0
#define APLL_CON2_VAL	0x00000020		//reset value: ICP =2; RSEL = 0; PBIAS_CTRL =0, PBIAS_CTRL_EN = 0

/* MPLL_CON1	*/
#define MPLL_CON1_VAL   0x00A0FC00		//reset value; AFC_ENB =0; AFC = 0x0; LOCK_EN =0
#define MPLL_CON2_VAL	0x00000020		//reset value: ICP =2; RSEL = 0; PBIAS_CTRL =0, PBIAS_CTRL_EN = 0

/* BPLL_CON1	*/
#define BPLL_CON1_VAL	0x00A0FC00		//reset value; AFC_ENB =0; AFC = 0x0; LOCK_EN =0
#define BPLL_CON2_VAL	0x00000020		//reset value: ICP =2; RSEL = 0; PBIAS_CTRL =0, PBIAS_CTRL_EN = 0

/* UPLL_CON1	*/
#define UPLL_CON1_VAL	0x00A0FC00		//reset value; AFC_ENB =0; AFC = 0x0; LOCK_EN =0
#define UPLL_CON2_VAL	0x00000020		//reset value: ICP =2; RSEL = 0; PBIAS_CTRL =0, PBIAS_CTRL_EN = 0


/*
**	EPLL/VPLL setting	
*/
#if defined(CONFIG_CLK_EPLL_800)
#if defined (CONFIG_SYS_FIN_26)
#define EPLL_MDIV	0xB9
#define EPLL_PDIV	0x3
#define EPLL_SDIV	0x1
#define EPLL_K		0x9D8A
#endif
#if defined (CONFIG_SYS_FIN_24)
#define EPLL_MDIV	0xC8
#define EPLL_PDIV	0x3
#define EPLL_SDIV	0x1
#define EPLL_K		0x0
#endif
#endif

#if defined(CONFIG_CLK_EPLL_200)
#if defined (CONFIG_SYS_FIN_26)
#define EPLL_MDIV	0xB9
#define EPLL_PDIV	0x3
#define EPLL_SDIV	0x3
#define EPLL_K		0x9D8A
#endif
#if defined (CONFIG_SYS_FIN_24)
#define EPLL_MDIV	0xC8
#define EPLL_PDIV	0x3
#define EPLL_SDIV	0x3
#define EPLL_K		0x0
#endif
#endif


#define EPLL_CON1_VAL	((0xC601<<16) | (EPLL_K))
#define EPLL_CON2_VAL	0x18000000		@reset value

#if defined(CONFIG_CLK_VPLL_335)
#if defined (CONFIG_SYS_FIN_26)
#define VPLL_MDIV	0x9B
#define VPLL_PDIV	0x3
#define VPLL_SDIV	0x2
#define VPLL_K		0x9D8A
#endif
#if defined (CONFIG_SYS_FIN_24)
#define VPLL_MDIV	0x70
#define VPLL_PDIV	0x2
#define VPLL_SDIV	0x2
#define VPLL_K		0xAAAB
#endif
#endif

#if defined(CONFIG_CLK_VPLL_266)
#if defined (CONFIG_SYS_FIN_24)
#define VPLL_MDIV	0x10A
#define VPLL_PDIV	0x3
#define VPLL_SDIV	0x3
#define VPLL_K		0x0
#endif
#endif

#define VPLL_CON1_VAL	((0xC601<<16) | (VPLL_K))
#define VPLL_CON2_VAL	0x18000000					//reset value

/********************************************************/

/* Set PLL	*/
#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_CON0_VAL	set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_CON0_VAL	set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)
#define BPLL_CON0_VAL	set_pll(BPLL_MDIV,BPLL_PDIV,BPLL_SDIV)
#define UPLL_CON0_VAL	set_pll(UPLL_MDIV,UPLL_PDIV,UPLL_SDIV)
#define EPLL_CON0_VAL	set_pll(EPLL_MDIV,EPLL_PDIV,EPLL_SDIV)
#define VPLL_CON0_VAL	set_pll(VPLL_MDIV,VPLL_PDIV,VPLL_SDIV)

/*
** CPU Clock
*/
/* CLK_SRC_CPU	*/
#define MUX_HPM_SEL_MOUTAPLL							0
#define MUX_HPM_SEL_SCLKMPLL_USER_CPU				1
#define MUX_CORE_SEL_MOUTAPLL						0
#define MUX_CORE_SEL_SCLKMPLL_USER_CPU				1

/* 0 = FILPLL, 1 = MOUT */
#define MUX_MPLL_USER_SEL_FINPLL	        	0
#define MUX_MPLL_USER_SEL_SCLKMPLL	 	1
#define MUX_APLL_SEL_FINPLL	        			0
#define MUX_APLL_SEL_APLLFOUT				1
#define CLK_SRC_CPU_VAL_FINPLL	        ((MUX_MPLL_USER_SEL_FINPLL << 24)    \
						| (MUX_HPM_SEL_MOUTAPLL << 20)   \
                                        | (MUX_CORE_SEL_MOUTAPLL <<16)  \
                                        | (MUX_APLL_SEL_FINPLL))
#define CLK_SRC_CPU_VAL_MOUTAPLL    ((MUX_MPLL_USER_SEL_SCLKMPLL << 24)  \
						| (MUX_HPM_SEL_MOUTAPLL << 20)           \
                                        | (MUX_CORE_SEL_MOUTAPLL <<16)          \
                                        | (MUX_APLL_SEL_APLLFOUT <<0))
/* CLK_DIV_CPU0	*/
#define CORE2_RATIO		0x0				 
#define APLL_RATIO			0x1
#define PCLK_DBG_RATIO		0x7
#define ATB_RATIO			0x3
#define COREM_RATIO		0x1
#define CORE_RATIO			0x0
#define CLK_DIV_CPU0_VAL        ( (CORE2_RATIO<<28) \
				     |(APLL_RATIO << 24)     \
                                | (PCLK_DBG_RATIO << 20)\
                                | (ATB_RATIO << 16)     \
                                | (COREM_RATIO << 4)   \
                                | (CORE_RATIO))

/* CLK_DIV_CPU1	*/
#define HPM_RATIO	        0x0
#define COPY_RATIO	        0x2
#define CLK_DIV_CPU1_VAL	((HPM_RATIO << 4) | (COPY_RATIO))

/*
** DMC Clock
**/
/* CLK_SRC_DMC	*/
#define MUX_MPLL_USR_SEL_FINPLL					        0
#define MUX_MPLL_USR_SEL_MPLLFOUT		 			 1
#define MUX_BPLL_SEL_FINPLL	        					 0
#define MUX_BPLL_SEL_FOUTBPLL							 1
#define MUX_DMC_DPHY_SEL_MOUTMPLL					 0
#define MUX_DMC_DPHY_SEL_MOUTBPLL					 1
#define MUX_DMC_BUS_SEL_MOUTMPLL					 0
#define MUX_DMC_BUS_SEL_MOUTBPLL					 1
#define MUC_AUDIOCODEC_SEL_MPLL						 0
#define MUC_AUDIOCODEC_SEL_EPLL						 1
#define MUC_C2C_SEL_MPLL						 		 0
#define MUC_C2C_SEL_BPLL						 		 1
#define CLK_SRC_DMC_VAL_FINPLL	        ((MUX_MPLL_USR_SEL_MPLLFOUT << 12)   \
                                        | (MUX_BPLL_SEL_FINPLL <<10)  \
                                        | (MUX_DMC_DPHY_SEL_MOUTBPLL << 8)    \
                                        | (MUX_DMC_BUS_SEL_MOUTBPLL<<4)	\
                                        | (MUC_AUDIOCODEC_SEL_EPLL<<2)	\
                                        | (MUC_C2C_SEL_MPLL<<0))
#define CLK_SRC_DMC_VAL_FOUTBPLL    ((MUX_MPLL_USR_SEL_MPLLFOUT << 12)           \
                                        | (MUX_BPLL_SEL_FOUTBPLL <<10)  \
                                        | (MUX_DMC_DPHY_SEL_MOUTBPLL << 8)    \
                                        | (MUX_DMC_BUS_SEL_MOUTBPLL<<4)	\
                                        | (MUC_AUDIOCODEC_SEL_EPLL<<2)	\
                                        | (MUC_C2C_SEL_MPLL<<0))

/* CLK_DIV_DMC1	*/
#define BPLL_RATIO_PRE		0x1
#define DMC_RATIO_SLEEP	0x3
#define DMC_RATIO			0x1
#define DPHY_RATIO			0x1
#define DMC_RATIO_PRE		0x0
#define DMCP_RATIO			0x1
#define DMCD_RATIO			0x1
#define AUDIOCODEC_RATIO	0xF
#define CLK_DIV_DMC1_VAL	( (DMC_RATIO << 27)   \
				| (DPHY_RATIO << 23)	\
				| (DMC_RATIO_PRE << 19)	\
				| (DMCP_RATIO << 15)	\
				| (DMCD_RATIO << 11)	\
				| (AUDIOCODEC_RATIO))
#define CLK_DIV_DMC1_VAL_SLEEP	((BPLL_RATIO_PRE << 30) \
				| (DMC_RATIO_SLEEP << 27)   \
				| (DPHY_RATIO << 23)	\
				| (DMC_RATIO_PRE << 19)	\
				| (DMCP_RATIO << 15)	\
				| (DMCD_RATIO << 11)	\
				| (AUDIOCODEC_RATIO))
#define CLK_DIV_DMC1_VAL_50M	((BPLL_RATIO_PRE << 30) \
				| (DMC_RATIO_SLEEP << 27)   \
				| (DPHY_RATIO << 23)	\
				| (DMC_RATIO_PRE << 19)	\
				| (3 << 15)	\				
				| (DMCD_RATIO << 11)	\
				| (AUDIOCODEC_RATIO))


/*
** ACP Clock
*/
/* CLK_SRC_ACP	*/
#define MUX_G2D_ACP_SEL								0x0
#define MUX_G2D_ACP_1_SEL								0x0
#define MUX_G2D_ACP_0_SEL								0x0
#define MUX_PWI_SEL	       							0x0
#define MUX_MPLL_USER_ACP_SEL_FINPLL					0x0
#define MUX_MPLL_USER_ACP_SEL_SCLKMPLL				0x1
#define MUX_BPLL_USER_ACP_SEL_FINPLL					0x0
#define MUX_BPLL_USER_ACP_SEL_SCLKBPLL				0x1
#define MUX_DMC_BUS_ACP_SEL_SCLKMPLL_USER_ACP		0x0
#define MUX_DMC_BUS_ACP_SEL_SCLKBPLL_USER_ACP		0x1
#define CLK_SRC_ACP_VAL_0		0x00010000
#define CLK_SRC_ACP_VAL_1		0x00012800		/*USER_ACP = Sclk*/
/* CLK_DIV_ACP	*/
#define ACP_DMC_PRE_RATIO			0x0		
#define ACP_DMCP_RATIO				0x1
#define ACP_DMC_RATIO				0x1
#define ACP_PCLK_RATIO				0x1
#define ACP_RATIO					0x3
#define CLK_DIV_ACP0_VAL	((ACP_DMC_PRE_RATIO << 16) \
				| (ACP_DMCP_RATIO << 12)   \
				| (ACP_DMC_RATIO << 8)	\
				| (ACP_PCLK_RATIO << 4)	\
				| (ACP_RATIO << 12))
#define	ACP_PWI_RATIO				0x0
#define 	ACP_G2D_RATIO				0x3
#define CLK_DIV_ACP1_VAL	((ACP_PWI_RATIO << 5) \
				| (ACP_G2D_RATIO << 12))
/********************************************************
 * TOP Clock
 ********************************************************/
/* CLK_SRC_TOP0	*/
#define MUX_EBI_SEL         0x0     		/* 0 = DOUT200, 1 = DOUT160		*/
#define MUX_ACLK_266_SEL	0x0		/* 0 = SCLK_MPLL*/
#define MUX_ACLK_160_SEL	0x0		/* 0 = SCLK_MPLL*/	
#define MUX_ACLK_100_SEL	0x0		/* 0 = SCLK_MPLL*/
#define MUX_ACLK_200_SEL	0x0		/* 0 = SCLK_MPLL*/
#define MUX_ACLK_266_1_SEL	0x0		/* 0 = SCLK_EPLL*/
#define MUX_ACLK_266_0_SEL	0x0		/* 0 = SCLK_MPLL*/
#define MUX_ACLK_266_SEL	0x0		/* 0 = ACLK_266_0*/
#define MUX_VPLL_SEL	        0x0		/* 0 = FIN*/
#define MUX_EPLL_SEL	        0x0		/* 0 = FIN*/
#define MUX_EBI1_SEL		 0x0		/* 0 = MOUTEBI*/
#define CLK_SRC_TOP0_VAL_0	((MUX_EBI_SEL << 28)	\
				| (MUX_ACLK_200_SEL << 24)	\
				| (MUX_ACLK_160_SEL << 20)	\
				| (MUX_ACLK_100_SEL << 16)	\
				| (MUX_ACLK_266_1_SEL << 14)	\
				| (MUX_ACLK_266_0_SEL << 13)	\
				| (MUX_ACLK_266_SEL << 12)	\
				| (MUX_VPLL_SEL << 8)	       \
				| (MUX_EPLL_SEL << 4)		\
				| (MUX_EBI1_SEL )	)
#define CLK_SRC_TOP0_VAL_1 0x00000110			/*EPLL VPLL = Fout*/

/* CLK_SRC_TOP1	*/
#define MUX_UPLL_SEL_FIN					0x0			/*MOUT_UPLL = FIN*/
#define MUX_UPLL_SEL_FOUTUPLL				0x1			/*MOUT_UPLL = FOUT_UPLL*/
#define MUX_ACLK_400_MCUISP_SUB_SEL		0x1			/*= ACLK400_ISP*/
#define MUX_ACLK_266_SUB_SEL				0x1			/*= ACLK266_ISP*/
#define MUX_MPLL_SEL_FIN					0x0			/*MOUT_MPLL = FIN*/
#define MUX_MPLL_SEL_FOUTMPLL				0x1			/*MOUT_MPLL = FOUT_MPLL*/
#define MUX_ACLK_400_MCUISP_SEL			0x0			/*= SCLK_MPLL */
#define MUX_VPLL_SRC_SEL					0x0			/*src = FIN*/
#define CLK_SRC_TOP1_VAL_0		((MUX_UPLL_SEL_FIN << 28)	\
				| (MUX_ACLK_400_MCUISP_SUB_SEL << 24)	\
				| (MUX_ACLK_266_SUB_SEL << 20)	\
				| (MUX_MPLL_SEL_FIN << 12)	\
				| (MUX_ACLK_400_MCUISP_SEL << 8)	\
				| (MUX_VPLL_SRC_SEL )	)
#define CLK_SRC_TOP1_VAL_1		((MUX_UPLL_SEL_FIN << 28)	\
				| (MUX_ACLK_400_MCUISP_SUB_SEL << 24)	\
				| (MUX_ACLK_266_SUB_SEL << 20)	\
				| (MUX_MPLL_SEL_FOUTMPLL << 12)	\
				| (MUX_ACLK_400_MCUISP_SEL << 8)	\
				| (MUX_VPLL_SRC_SEL )	)


#define CLK_SRC_MFC_VAL	0x0
#define CLK_SRC_G3D_VAL	0x0
/* CLK_DIV_TOP	*/
#define MPLL_RATIO_PRE	0x0			/*SCLK_MPLL = MOUTPLL/2*/
#define ACLK_400_RATIO	0x1
#define EBI_RATIO		0x0
#define ACLK_200_RATIO	0x3
#define ACLK_160_RATIO	0x4
#define ACLK_100_RATIO	0x7
#define ACLK_266_RATIO	0x2
#define CLK_DIV_TOP_VAL	((MPLL_RATIO_PRE<<28)\
			| (ACLK_400_RATIO<<24)\
			| (EBI_RATIO << 16) \
			| (ACLK_200_RATIO << 12)	\
			| (ACLK_160_RATIO << 8)	\
			| (ACLK_100_RATIO << 4)	\
			| (ACLK_266_RATIO))
#define CLK_DIV_G3D_VAL	0x0
#define CLK_DIV_MFC_VAL	0x0
/********************************************************
 * LEFTBUS Clock
 ********************************************************/
/* CLK_SRC_LEFTBUS	*/
#define MUX_MPLL_USER_L_SEL_SCLKMPLL	       0x1
#define MUX_MPLL_USER_L_SEL_FINPLL	       0x0
#define MUX_GDL_SEL	        		0x0
#define CLK_SRC_LEFTBUS_VAL	((MUX_MPLL_USER_L_SEL_SCLKMPLL<<4)  | (MUX_GDR_SEL))

/* CLK_DIV_LEFRBUS	*/
#define GPL_RATIO	        0x1
#define GDL_RATIO	        0x3
#define CLK_DIV_LEFTBUS_VAL	((GPL_RATIO << 4) \
				| (GDL_RATIO))
/********************************************************
 * RIGHT Clock
 ********************************************************/
/* CLK_SRC_RIGHTBUS	*/
#define MUX_MPLL_USER_R_SEL	       0x1				//select SLCK_MPLL
#define MUX_GDR_SEL				0x0
#define CLK_SRC_RIGHTBUS_VAL	((MUX_MPLL_USER_R_SEL<<4)  | (MUX_GDR_SEL))

/* CLK_DIV_RIGHTBUS	*/
#define GPR_RATIO	        0x1
#define GDR_RATIO	        0x3
#define CLK_DIV_RIGHTBUS_VAL	((GPR_RATIO << 4) \
				| (GDR_RATIO))


/* CLK_SRC_PERIL0	*/
#ifdef CONFIG_SMDK3472_FPGA_DEBUG
#define UART4_SEL	0
#define UART3_SEL	0
#define UART2_SEL	0
#define UART1_SEL	0
#define UART0_SEL	0
#else
#if defined (CONFIG_SPL_BUILD)
#define UART4_SEL	0
#define UART3_SEL	0
#define UART2_SEL	0
#define UART1_SEL	0
#define UART0_SEL	0
#else
#define UART4_SEL	6
#define UART3_SEL	6
#define UART2_SEL	6
#define UART1_SEL	6
#define UART0_SEL	6
#endif
#endif
#define CLK_SRC_PERIL0_VAL	(  (UART4_SEL << 16) \
				| (UART3_SEL << 12) \
				| (UART2_SEL<< 8)	\
				| (UART1_SEL << 4)	\
				| (UART0_SEL))




/* CLK_DIV_PERIL0	*/
#ifdef CONFIG_SMDK3472_FPGA_DEBUG
#define UART4_RATIO	0
#define UART3_RATIO	0
#define UART2_RATIO	0
#define UART1_RATIO	0
#define UART0_RATIO	0
#else
#if defined (CONFIG_SPL_BUILD)
#define UART4_RATIO	0x0
#define UART3_RATIO	0x0
#define UART2_RATIO	0x0
#define UART1_RATIO	0x0
#define UART0_RATIO	0x0
#else
#define UART4_RATIO	0xF
#define UART3_RATIO	0xF
#define UART2_RATIO	0xF
#define UART1_RATIO	0xF
#define UART0_RATIO	0xF
#endif
#endif
#define CLK_DIV_PERIL0_VAL	( (UART4_RATIO << 16) \
				| (UART3_RATIO << 12)	\
				| (UART2_RATIO << 8)	\
				| (UART1_RATIO << 4)	\
				| (UART0_RATIO))



/*******************************************************************************
 * UART
 ******************************************************************************/
#define UART0_OFFSET			0x00000
#define UART1_OFFSET			0x10000
#define UART2_OFFSET			0x20000
#define UART3_OFFSET			0x30000

#define EXYNOS3472_UART_BASE       	0x13800000

#if defined(CONFIG_SERIAL0)
#define UART_CONSOLE_BASE (EXYNOS3472_UART_BASE + UART0_OFFSET)
#elif defined(CONFIG_SERIAL1)
#define UART_CONSOLE_BASE (EXYNOS3472_UART_BASE + UART1_OFFSET)
#elif defined(CONFIG_SERIAL2)
#define UART_CONSOLE_BASE (EXYNOS3472_UART_BASE + UART2_OFFSET)
#elif defined(CONFIG_SERIAL3)
#define UART_CONSOLE_BASE (EXYNOS3472_UART_BASE + UART3_OFFSET)
#else
#define UART_CONSOLE_BASE (EXYNOS3472_UART_BASE + UART0_OFFSET)
#endif

#define ULCON_OFFSET			0x00
#define UCON_OFFSET			0x04
#define UFCON_OFFSET			0x08
#define UMCON_OFFSET			0x0C
#define UTRSTAT_OFFSET			0x10
#define UERSTAT_OFFSET			0x14
#define UFSTAT_OFFSET			0x18
#define UMSTAT_OFFSET			0x1C
#define UTXH_OFFSET			0x20
#define URXH_OFFSET			0x24
#define UBRDIV_OFFSET			0x28
#define UDIVSLOT_OFFSET			0x2C
#define UINTP_OFFSET			0x30
#define UINTSP_OFFSET			0x34
#define UINTM_OFFSET			0x38
#define UART_ERR_MASK			0xF


/* UART */
#define MPLL_DEC (MPLL_MDIV * FIN / (MPLL_PDIV * 2^(MPLL_SDIV)))
#define SCLK_UART               MPLL_DEC/ (UART1_RATIO+1)

#if defined(CONFIG_SMDK3472_FPGA_DEBUG)
/* (SCLK_UART/(115200*16) -1) */
#define UART_UBRDIV_VAL			0x5		/*115200 @ SCLK_UART = 12MHz*/
/*((((SCLK_UART*10/(115200*16) -10))%10)*16/10)*/
#define UART_UDIVSLOT_VAL		0x8
#else
#if defined (CONFIG_SPL_BUILD)
#if defined (CONFIG_SYS_FIN_26)
#define UART_UBRDIV_VAL			0xD		/*115200 @ SCLK_UART = 26MHz*/
#define UART_UDIVSLOT_VAL		0x1
#endif
#if defined (CONFIG_SYS_FIN_24)
#define UART_UBRDIV_VAL			0xC		/*115200 @ SCLK_UART = 24MHz*/
#define UART_UDIVSLOT_VAL		0x0
#endif
#else
#if defined (CONFIG_CLK_MPLL_1600)
#define UART_UBRDIV_VAL			0x1A	/*115200 @ SCLK_UART = 1600/2/16 = 50MHz*/
#define UART_UDIVSLOT_VAL		0x2
#endif
#endif
#endif

#define PSHOLD_CONTROL_OFFSET	0x330C



/*******************************************************************************
 * End of smdk3472_val.h
 ******************************************************************************/
#endif

