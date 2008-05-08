/*
      _   _  ____   _____ 
     | | (_)/ __ \ / ____|
  ___| |_ _| |  | | (___  
 / _ \ __| | |  | |\___ \  Copyright 2008 Daniel Bader, Vincenz Doelle,
|  __/ |_| | |__| |____) |        Johannes Schamburger, Dmitriy Traytel
 \___|\__|_|\____/|_____/ 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file 
 * Function to print things on the monitor
 *
 * date          stat
 * -----------------------------------------------
 * 01.05.08      first just-playing-version (DT)
 * 02.05.08      positioning fixes
 *  	         put* (it's a regex :) ) functions write only to the display-memory (B8000-B8FA0) (DT)
 * 04.05.08      updated types (from types.h)
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 *
 */
 
//colors macros
#include "../include/types.h"

enum{
        BLACK=0x0,
        BLUE=0x1,
        GREEN=0x2,
        CYAN=0x3,
        RED=0x4,
        VIOLET=0x5,
        ORANGE=0x6,
        LIGHTGREY=0x7,
        DARKGREY=0x8,
        LIGHTBLUE=0x9,
        LIGHTGREEN=0xA,
        TURQUOISE=0xB,
        PINK=0xC,
        MAGENTA=0xD,
        YELLOW=0xE,
        WHITE=0xF,
};

static uint16 *disp = (uint16*)0xB8000; //display pointer

/**
 *  writes a colored character to the display (fg=foregroung, bg=background)
 */	
void putc_col(uint8 ch, uint8 fg, uint8 bg)
{
        uint32 i=0;
        uint32 temp;
        if((uint32)disp >= 0xB8FA0)disp-=0x7D0; //return to beginning, if outside of display-memory
        switch(ch){
        case '\n':
                temp= 0x50 - (((uint32)disp - 0xB8000) % 0xA0) / 2;
                while(i < temp){ //calculating the "new line" starting position
                        i++;
                        putc_col(' ',WHITE,BLACK);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - 0xB8000) % 0x10) / 2;
                while(i < temp){ //calculating the "next tab" starting position
                        i++;
                        putc_col(' ',WHITE,BLACK);
                }
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                disp += 1;
	}
}

/*
 *  writes colored a string to the display (fg=foregroung, bg=background)
 */
void puts_col(uint8* str, uint8 fg, uint8 bg)
{
        while(*str!=0){
                putc_col(*str,fg,bg);
                str += 1;
        }
}

/*
 * writes a character to the display
 */
void putc(uint8 ch)
{
        putc_col(ch,WHITE,BLACK);
}

/*
 * writes a string to the display
 */
void puts(uint8* str)
{
        puts_col(str,WHITE,BLACK);
}

/*
 * writes an integer to the display
 */
void puti(sint32 x)
{
        int div=1000000000;
        if(x < 0){
                putc_col('-',RED,BLACK);
                x = -x;
        }
        else if (x == 0) {putc_col('0',RED,BLACK); return;}
        while(div > x)
                        div /= 10;
        while(div > 0){
                putc_col((uint8)(x / div + 48),RED,BLACK);
                x%=div;
                div /= 10;
        }    
}

/*
 * writes a hex-byte to the display
 */
void puthex(uint8 ch)
{
        uint8 low = ch % 16;
        uint8 high = ch / 16;  
        if(high>9)
                putc_col((high + 55),GREEN,BLACK);
        else
                putc_col((high + 48),GREEN,BLACK);   
        if(low>9)
                putc_col((low + 55),GREEN,BLACK);
        else
                putc_col((low + 48),GREEN,BLACK); 
        putc_col('h',GREEN,BLACK);
}

void drawtest() //output-testing
{ 
        puts_col("Test by Dmitriy Traytel (the i/o-master of etiOS)\n",WHITE,BLACK); 
        puts("      _   _  ____   _____\n"); 
	puts("     | | (_)/ __ \\ / ____|\n");
        puts("  ___| |_ _| |  | | (___\n");
        puts(" / _ \\ __| | |  | |\\___ \\\n"); 
        puts("|  __/ |_| | |__| |____) |\n");
        puts(" \\___|\\__|_|\\____/|_____/\n\n");
        puts_col("Copyright 2008 Daniel Bader,Vincenz Doelle,Johannes Schamburger,Dmitriy Traytel\n",0xA,BLACK);
        
        puts("\ttab-test\tok\n");
        puti(strlen("hallo welt")); //strlen-test
        
        //some ints
        puts("\t");
        puti(0);
        puts("\t");
        puti(100);
        puts("\t");
        puti(-2);
        puts("\t");
        
        //some hex-bytes
        uint8 *test=(uint8*)0x60;
        puthex(*test);                  //0x60h
        puthex(*(test+1));              //0x61h
        puthex(*(test+2));              //0x62h
        puthex(*(test+3));              //0x63h
        puthex(*(test+4));              //0x64h
        puthex(*(test+5));              //0x65h
        
        puts("\t\n");
        puts("\tnumber-test\tok\n");
        
        puts("\tbegin color test:\n");
        puts_col("                                        ",BLACK,WHITE);
        puts_col("                                        ",BLACK,BLUE); 
        puts_col("                                        ",BLACK,GREEN); 
        puts_col("                                        ",BLACK,CYAN); 
        puts_col("                                        ",BLACK,RED); 
        puts_col("                                        ",BLACK,VIOLET);
        puts_col("                                        ",BLACK,ORANGE); 
        puts_col("                                        ",BLACK,LIGHTGREY); 
        puts_col("                                        ",BLACK,DARKGREY); 
        puts_col("                                        ",BLACK,LIGHTBLUE); 
        puts_col("                                        ",BLACK,LIGHTGREEN);
        puts_col("                                        ",BLACK,TURQUOISE); 
        puts_col("                                        ",BLACK,PINK); 
        puts_col("                                        ",BLACK,MAGENTA); 
        puts_col("                                        ",BLACK,YELLOW); 
        puts_col("                                        ",BLACK,BLACK);
        puts("\tcolor-test\tok\n");		
}
