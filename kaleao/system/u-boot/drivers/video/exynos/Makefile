include $(TOPDIR)/config.mk

LIB := $(obj)libdecon.o

#
#	CONFIG_EXYNOS_DECON (exynos7xxx)
#

#	Driver file configuration
COBJS-y = decon_core.o dsim_drv.o bootmode.o bootmode_decon.o
COBJS-$(CONFIG_CPU_EXYNOS7420) += decon_reg_7420.o dsim_reg_7420.o
COBJS-$(CONFIG_CPU_EXYNOS7580) += decon_reg_7580.o dsim_reg_7580.o

#	LCD file configuration
COBJS-$(CONFIG_DECON_LCD_S6E8FA0X01) += ./panels/s6e8fa0x01_mipi_lcd.o ./panels/s6e8fa0x01_lcd_ctrl.o
COBJS-$(CONFIG_DECON_LCD_S6E8AA0) += ./panels/s6e8aa0_mipi_lcd.o ./panels/s6e8aa0_lcd_ctrl.o
COBJS-$(CONFIG_DECON_LCD_S6E3FA0) += ./panels/s6e3fa0_mipi_lcd.o ./panels/s6e3fa0_lcd_ctrl.o
COBJS-$(CONFIG_DECON_LCD_S6TNMR7) += ./panels/s6tnmr7_mipi_lcd.o 

COBJS   := $(COBJS-y)
SRCS    := $(COBJS:.o=.c)
OBJS    := $(addprefix $(obj),$(COBJS))

all:    $(LIB)

$(LIB): $(obj).depend $(OBJS)
	  $(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
#COBJS-y = bootmode.o bootmode_decon.o
#COBJS   +=bootmode.o bootmode_decon.o
