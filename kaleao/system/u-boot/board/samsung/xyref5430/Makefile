#
# Copyright (C) 2012 Samsung Electronics
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).o

ifdef CONFIG_ARM64
SOBJS	:= lowlevel_init_aarch64.o
else
SOBJS	:= lowlevel_init.o
endif

ifdef	CONFIG_CPU_EXYNOS5433
COBJS	:= clock_init_5433.o
else
COBJS	:= clock_init.o
endif
ifdef	CONFIG_CPU_EXYNOS5433_EVT1
COBJS	+= dmc_init.o
else
COBJS	+= dmc_init_5430.o
endif
COBJS	+= pmic.o
ifdef CONFIG_ARM64
COBJS	+= smc_aarch64.o
else
COBJS	+= smc.o
endif
ifdef	CONFIG_CPU_EXYNOS5433
ifndef	CONFIG_CPU_EXYNOS5433_EVT1
COBJS	+= ASVGrpVoltageSetting_5433.o
endif
else
COBJS	+= ASVGrpVoltageSetting.o
endif

ifdef CONFIG_LCD
COBJS	+= display.o
endif

ifdef CONFIG_TZPC
COBJS	+= tzpc_init.o
endif

ifndef CONFIG_SPL_BUILD
COBJS	+= xyref5430.o
ifdef	CONFIG_CPU_EXYNOS5433_EVT1
COBJS	+= ASVGrpVoltageSetting_5433_evt1.o
COBJS	+= otp.o
endif
COBJS	+= pmic_lm3560.o
endif

ifdef CONFIG_SPL_BUILD
COBJS	+= mmc_boot.o
endif

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

ALL	:=	 $(obj).depend $(LIB)

all:	$(ALL)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
