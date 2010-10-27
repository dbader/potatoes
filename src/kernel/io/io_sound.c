/* $Id: io_sound.c 12 2009-11-30 18:52:20Z dtraytel $
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
 * Sound driver
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 12 $
 */
#include "../include/types.h"
#include "io_sound.h"
#include "io_timer.h"
#include "io.h"

const int notes[12][8] = {
        {16, 33, 65, 131, 262, 523, 1046, 2093},
        {17, 35, 69, 139, 277, 554, 1109, 2217},
        {18, 37, 73, 147, 294, 587, 1175, 2349},
        {19, 39, 78, 155, 311, 622, 1244, 2489},
        {21, 41, 82, 165, 330, 659, 1328, 2637},
        {22, 44, 87, 175, 349, 698, 1397, 2794},
        {23, 46, 92, 185, 370, 740, 1480, 2960},
        {24, 49, 98, 196, 392, 784, 1568, 3136},
        {26, 52, 104, 208, 415, 831, 1661, 3322},
        {27, 55, 110, 220, 440, 880, 1760, 3520},
        {29, 58, 116, 233, 466, 932, 1865, 3729},
        {31, 62, 123, 245, 494, 988, 1975, 3951},
};

const char *note_names[12] = {
	"C",
	"CSH",
	"D",
	"DSH",
	"E",
	"F",
	"FSH",
	"G",
	"GSH",
	"A",
	"ASH",
	"B"
};

void start_beep(uint32 freq){
        uint16 note = (uint16)(1193180 / freq);
        outb(PIT_CONTROL, PIT_SOUND_CMD);
        outb(PIT_COUNTER2, note % 0xFF); //LSB
        outb(PIT_COUNTER2, note / 0xFF); //MSB

        uint8 tmp = inb(IO_SPEAKER_PORT);
        if (tmp != (tmp | 3)) {
                outb(IO_SPEAKER_PORT, tmp | 3);
        }
}

void end_beep() {
        outb(IO_SPEAKER_PORT, inb(IO_SPEAKER_PORT) & 0xFC);
}
