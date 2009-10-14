/* $Id$
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

/**
 * @file
 * Header file for the games.
 *
 * @author Daniel Bader
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#ifndef __GAMES_H
#define __GAMES_H

#include "../kernel/include/types.h"
#include "../kernel/io/io_sound.h"

void shell_cmd_pong(int argc, char *argv[]);
void shell_cmd_snake(int argc, char *argv[]);
void shell_cmd_synth(int argc, char *argv[]);
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
#define ENTER 0x1C
#define SHARP 0x2B
#define KEY_Q 0x10
#define KEY_W 0x11
#define KEY_E 0x12
#define KEY_R 0x13
#define KEY_T 0x14
#define KEY_Z 0x15
#define KEY_U 0x16
#define KEY_I 0x17
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_F 0x21
#define KEY_G 0x22
#define KEY_H 0x23
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26
#define KEY_PLUS 0x1B
#define KEY_MINUS 0x35

#define SET_PIXEL(x,y,cl) bbuf[(y)*80+(x)] = cl;

#define DRAW_PADDLE(x,y,cl) SET_PIXEL(x,y,cl); SET_PIXEL(x,y+1,cl); SET_PIXEL(x,y+2,cl); \
        SET_PIXEL(x,y+3,cl); SET_PIXEL(x,y+4,cl);

#define LIMIT(x, min, max) if (x<min) x = min; if (x>max) x = max;

#define HIT_PADDLE(paddley, y) (paddley <= (y) && paddley + 5 >= (y))

#define PADDLE_DEFLECTION(paddley, hit) (paddley - (hit)) * 10

#define HIT_RPADDLE_SOUND NOTE_B4
#define HIT_LPADDLE_SOUND NOTE_C4
#define HIT_SIDE_SOUND NOTE_E4

// 4x5 px font
typedef uint8 glyph_t[4*5];
static glyph_t font[] = {
        { // 0
                1, 1, 1, 1,
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 1, 1, 1
        },
        { // 1
                0, 0, 1, 0,
                0, 0, 1, 0,
                0, 0, 1, 0,
                0, 0, 1, 0,
                0, 0, 1, 0
        },
        { // 2
                1, 1, 1, 1,
                0, 0, 0, 1,
                1, 1, 1, 1,
                1, 0, 0, 0,
                1, 1, 1, 1
        },
        { // 3
                1, 1, 1, 1,
                0, 0, 0, 1,
                1, 1, 1, 1,
                0, 0, 0, 1,
                1, 1, 1, 1
        },
        { // 4
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 1, 1, 1,
                0, 0, 0, 1,
                0, 0, 0, 1
        },
        { // 5
                1, 1, 1, 1,
                1, 0, 0, 0,
                1, 1, 1, 1,
                0, 0, 0, 1,
                1, 1, 1, 1
        },
        { // 6
                1, 1, 1, 1,
                1, 0, 0, 0,
                1, 1, 1, 1,
                1, 0, 0, 1,
                1, 1, 1, 1
        },
        { // 7
                1, 1, 1, 1,
                0, 0, 0, 1,
                0, 0, 0, 1,
                0, 0, 0, 1,
                0, 0, 0, 1
        },
        { // 8
                1, 1, 1, 1,
                1, 0, 0, 1,
                1, 1, 1, 1,
                1, 0, 0, 1,
                1, 1, 1, 1
        },
        { // 9
                1, 1, 1, 1,
                1, 0, 0, 1,
                1, 1, 1, 1,
                0, 0, 0, 1,
                1, 1, 1, 1
        }
};

#define DRAW_GLYPH(x, y, idx, cl) for (int px = 0; px < sizeof(glyph_t); px++) \
        SET_PIXEL(x + px % 4, y + px / 4, (font[idx][px] ? cl : 0));

#define SNAKE_RIGHT 1
#define SNAKE_LEFT 2
#define SNAKE_UP 3
#define SNAKE_DOWN 4
#define EAT_APPLE_SOUND NOTE_C5

#endif /*__GAMES_H*/
