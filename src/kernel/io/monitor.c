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

static unsigned short *disp = (unsigned short*)0xB8000;
	
void putc_col(char ch, short fg, short bg)
{
        if((unsigned int)disp >= 0xB8FA0) disp=(unsigned short*)0xB8000;
        switch(ch){
        case '\n':
                disp += 0x50 - (((unsigned int)disp - 0xB8000) % 0xA0) / 2;
                break;
        case '\t':
                disp += 0x8 - (((unsigned int)disp - 0xB8000) % 0x10) / 2;
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch;
                disp += 1;
	}
}

void puts_col(char* str, short fg, short bg)
{
        while(*str!=0){
                putc_col(*str,fg,bg);
                str += 1;
        }
}

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
        puts_col("Test by Dmitriy Traytel (the i/o-master of etiOS)                               ",WHITE,BLACK); 
        puts("      _   _  ____   _____\n"); 
	puts("     | | (_)/ __ \\ / ____|\n");
        puts("  ___| |_ _| |  | | (___\n");
        puts(" / _ \\ __| | |  | |\\___ \\\n"); 
        puts("|  __/ |_| | |__| |____) |\n");
        puts(" \\___|\\__|_|\\____/|_____/\n\n");
        puts_col("Copyright 2008 Daniel Bader,Vincenz Doelle,Johannes Schamburger,Dmitriy Traytel\n",0xA,BLACK);
        puts("\ttab-test\tok\n");
        puts("\tbegin color test:\n");
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
        puts_col("                                        \n",BLACK,BLACK);
        puts("\tcolor-test\tok\n");
	
}
