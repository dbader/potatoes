/* $Id: brainfuck_interpreter.c 195 2008-08-29 22:54:26Z dtraytel $
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
 * Header file for the games.
 *
 * @author Daniel Bader
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 195 $
 */
#ifndef __GAMES_H
#define __GAMES_H

#include "../kernel/include/types.h"

void shell_cmd_pong(int argc, char *argv[]);
void shell_cmd_snake(int argc, char *argv[]);
bool keydown(char key, int fd);

//bool keydown(char key)
//{
//        char ch = 0;
//        _read(STDIN, &ch, sizeof(ch));
//        return ch == key;
//}


/*
 * PONG code.
 * I know this is shitty but I really had to do this quickly :)
 * Have fun!
 */

#define CURSOR_UP 0x48
#define CURSOR_DOWN 0x50
#define CURSOR_LEFT 0x4B
#define CURSOR_RIGHT 0x4D
#define ESCAPE 0x01
#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_I 0x17
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26

#define SET_PIXEL(x,y,cl) bbuf[(y)*80+(x)] = cl;

#define DRAW_PADDLE(x,y,cl) SET_PIXEL(x,y,cl); SET_PIXEL(x,y+1,cl); SET_PIXEL(x,y+2,cl); \
        SET_PIXEL(x,y+3,cl); SET_PIXEL(x,y+4,cl);

#define LIMIT(x, min, max) if (x<min) x = min; if (x>max) x = max;

#define HIT_PADDLE(paddley, y) (paddley <= (y) && paddley + 5 >= (y))

#define PADDLE_DEFLECTION(paddley, hit) (paddley - (hit)) * 10

// 4x5 px font
typedef uint8 glyph_t[4*5];
static glyph_t font[] = {
                { // 0
                                1,1,1,1,
                                1,0,0,1,
                                1,0,0,1,
                                1,0,0,1,
                                1,1,1,1
                },
                { // 1
                                0,0,1,0,
                                0,0,1,0,
                                0,0,1,0,
                                0,0,1,0,
                                0,0,1,0
                },
                { // 2
                                1,1,1,1,
                                0,0,0,1,
                                1,1,1,1,
                                1,0,0,0,
                                1,1,1,1
                },
                { // 3
                                1,1,1,1,
                                0,0,0,1,
                                1,1,1,1,
                                0,0,0,1,
                                1,1,1,1
                },
                { // 4
                                1,0,0,1,
                                1,0,0,1,
                                1,1,1,1,
                                0,0,0,1,
                                0,0,0,1
                },
                { // 5
                                1,1,1,1,
                                1,0,0,0,
                                1,1,1,1,
                                0,0,0,1,
                                1,1,1,1
                },
                { // 6
                                1,1,1,1,
                                1,0,0,0,
                                1,1,1,1,
                                1,0,0,1,
                                1,1,1,1
                },
                { // 7
                                1,1,1,1,
                                0,0,0,1,
                                0,0,0,1,
                                0,0,0,1,
                                0,0,0,1
                },
                { // 8
                                1,1,1,1,
                                1,0,0,1,
                                1,1,1,1,
                                1,0,0,1,
                                1,1,1,1
                },
                { // 9
                                1,1,1,1,
                                1,0,0,1,
                                1,1,1,1,
                                0,0,0,1,
                                1,1,1,1
                }
};

#define DRAW_GLYPH(x, y, idx, cl) for (int px = 0; px < sizeof(glyph_t); px++) \
        SET_PIXEL(x + px % 4, y + px / 4, (font[idx][px] ? cl : 0));

#define SNAKE_RIGHT 1
#define SNAKE_LEFT 2
#define SNAKE_UP 3
#define SNAKE_DOWN 4

#endif /*__GAMES_H*/
