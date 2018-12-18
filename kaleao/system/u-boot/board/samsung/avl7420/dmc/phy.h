/*
 * Memory setup for EXYNOS7420
 *
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

#ifndef PHY_H_
#define PHY_H_

#if defined( PHY_V5_R1 ) || defined( PHY_V5_R2 )
	#include "phy_v5_r2.h"
#elif defined( PHY_V6_R1 )
	#include "phy_v6_r1.h"
#elif defined( PHY_V7_R0 )
	#include "phy_v7_r0.h"
#else
	#error
#endif

#ifndef __ASSEMBLY__


#endif



#endif /* PHY_H_ */
