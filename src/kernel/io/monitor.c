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

//first just-playing-version (DT)

static unsigned short *disp = (unsigned short*)0xB8000;
	
void putc_col(char ch, short fg, short bg)
{
	*disp=bg*0x1000+fg*0x100+ch;
        disp+=1;
}

void puts_col(char* str, short fg, short bg)
{
	while(*str!=0){
        	putc_col(*str,fg,bg);
		str+=1;
        }
}

#define BLACK 0x0
#define WHITE 0xF

void putc(char ch)
{
	putc_col(ch,WHITE,BLACK);
}

void puts(char* str)
{
	puts_col(str,WHITE,BLACK);
}

void drawtest()
{ 
        puts_col("Test by Dmitriy Traytel                                                         ",WHITE,0x0); 
        puts_col("                                                                                ",BLACK,0x1); 
        puts_col("                                                                                ",BLACK,0x2); 
        puts_col("                                                                                ",BLACK,0x3); 
        puts_col("                                                                                ",BLACK,0x4); 
        puts_col("                                                                                ",BLACK,0x5);
        puts_col("                                                                                ",BLACK,0x6); 
        puts_col("                                                                                ",BLACK,0x7); 
        puts_col("                                                                                ",BLACK,0x8); 
        puts_col("                                                                                ",BLACK,0x9); 
        puts_col("                                                                                ",BLACK,0xA);
        puts_col("                                                                                ",BLACK,0xB); 
        puts_col("                                                                                ",BLACK,0xC); 
        puts_col("                                                                                ",BLACK,0xD); 
        puts_col("                                                                                ",BLACK,0xE); 
        puts_col("                                                                                ",BLACK,0xF);
	puts("      _   _  ____   _____                                                       "); 
	puts("     | | (_)/ __ \\ / ____|                                                      ");
        puts("  ___| |_ _| |  | | (___                                                        ");
        puts(" / _ \\ __| | |  | |\\___ \\                                                       "); 
        puts("|  __/ |_| | |__| |____) |                                                      ");
        puts(" \\___|\\__|_|\\____/|_____/                                                       "); 
        puts("                                                                                ");
        puts_col("Copyright 2008 Daniel Bader,Vincenz Doelle,Johannes Schamburger,Dmitriy Traytel ",0xA,BLACK);
}
 