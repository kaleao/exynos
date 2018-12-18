#
# Copyright (C) 2014 Samsung Electronics
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

CFLAGS += -Wundef -Wno-trigraphs -fno-strict-aliasing -Wno-format-security \
	-fdiagnostics-show-option -Wno-unused -fno-delete-null-pointer-checks

LIB	= $(obj)lib$(BOARD).o

SOBJS	:= lowlevel_init_aarch64.o

COBJS	:= main.o
COBJS	+= cmu.o
COBJS	+= dmc_init.o

ifndef CONFIG_SPL_BUILD
ifdef CONFIG_MUIC
COBJS	+= muic.o
endif
COBJS	+= if_pmic.o
endif

ifndef CONFIG_SPL_BUILD
ifdef CONFIG_RAMDUMP_GPR
COBJS	+= dfd.o
SOBJS	+= dfd_entry.o
endif
endif

ifndef CONFIG_SPL_BUILD
ifdef CONFIG_ETBDUMP_MODE
COBJS	+= coresight.o
endif
endif

COBJS	+= pmic.o

COBJS	+= smc_aarch64.o

ifndef CONFIG_SPL_BUILD
ifdef CONFIG_EXYNOS_LCD_ON_UBOOT
COBJS	+= display.o
endif
endif

ifndef CONFIG_SPL_BUILD
COBJS	+= smdk7580.o
endif

ifdef CONFIG_SPL_BUILD
COBJS	+= mmc_boot.o
endif

ifndef CONFIG_SPL_BUILD
COBJS	+= ldfw.o
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
