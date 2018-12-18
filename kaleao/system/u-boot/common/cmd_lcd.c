/*
 * (C) Copyright 2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <common.h>
#include <command.h>
#include <led-display.h>

//#undef DEBUG_LCD

extern void LCD_Turnoff(void);
int do_lcd (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
    char cmd;
    char arg[12];
	if (argc < 2)
		return (0);
#if 1    
    if(strcmp("off",argv[1])==0){
        printf("LCD_Turnoff\n");
        LCD_Turnoff();
    }else{
      printf("uknown command,Just only realize lcd off command\n");
    }
#endif    
#if 0
    cmd=argv[1][0];
    switch(cmd-'0')
    {
        case 0:
            printf("turnon\n");
            //LCD_turnon();
            break;
        case 1:
            printf("turnoff\n");
            LCD_Turnoff();
            break;
        case 2:
            printf("clear with black color\n");
            //LCD_clear_content();
            break;
        case 3:
            printf("showprog with 100%\n");
            //LCD_setprogress(100);
            break;
        case 4:
            printf("fill is not yet implemented\n");
            break;
        case 5:
            printf("sleep\n");
            //LCD_sleep();
            break;
        case 6:
            printf("resume\n");
            //LCD_resume();
            break;
        default:
            printf("unknown command\n");
            return (0);
    }
#endif
	return (0);
}

/***************************************************/

U_BOOT_CMD(
	lcd,	CONFIG_SYS_MAXARGS,	2,	do_lcd,
	"control lcd for eDP and MIPI",
	"<command number> [argument]\n"
	"    - LCD turn off : off\n"
	"    - only realize LCD turn off command\n"
);
