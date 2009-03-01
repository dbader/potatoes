/* $Id: io_sound.h 239 2008-10-26 23:19:15Z dtraytel $
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
 * Sound driver header
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 239 $
 */
#ifndef __IO_SOUND_H_
#define __IO_SOUND_H_

#define IO_SPEAKER_PORT 0x61

static const int notes[12][8] = {
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

#define NOTE_C 0
#define NOTE_CSH 1
#define NOTE_D 2
#define NOTE_DSH 3
#define NOTE_E 4
#define NOTE_F 5
#define NOTE_FSH 6
#define NOTE_G 7
#define NOTE_GSH 8
#define NOTE_A 9
#define NOTE_ASH 10
#define NOTE_B 11

#define NOTE_C0 notes[0][0]
#define NOTE_C1 notes[0][1]
#define NOTE_C2 notes[0][2]
#define NOTE_C3 notes[0][3]
#define NOTE_C4 notes[0][4]
#define NOTE_C5 notes[0][5]
#define NOTE_C6 notes[0][6]
#define NOTE_C7 notes[0][7]

#define NOTE_CSH0 notes[1][0]
#define NOTE_CSH1 notes[1][1]
#define NOTE_CSH2 notes[1][2]
#define NOTE_CSH3 notes[1][3]
#define NOTE_CSH4 notes[1][4]
#define NOTE_CSH5 notes[1][5]
#define NOTE_CSH6 notes[1][6]
#define NOTE_CSH7 notes[1][7]

#define NOTE_D0 notes[2][0]
#define NOTE_D1 notes[2][1]
#define NOTE_D2 notes[2][2]
#define NOTE_D3 notes[2][3]
#define NOTE_D4 notes[2][4]
#define NOTE_D5 notes[2][5]
#define NOTE_D6 notes[2][6]
#define NOTE_D7 notes[2][7]

#define NOTE_DSH0 notes[3][0]
#define NOTE_DSH1 notes[3][1]
#define NOTE_DSH2 notes[3][2]
#define NOTE_DSH3 notes[3][3]
#define NOTE_DSH4 notes[3][4]
#define NOTE_DSH5 notes[3][5]
#define NOTE_DSH6 notes[3][6]
#define NOTE_DSH7 notes[3][7]

#define NOTE_E0 notes[4][0]
#define NOTE_E1 notes[4][1]
#define NOTE_E2 notes[4][2]
#define NOTE_E3 notes[4][3]
#define NOTE_E4 notes[4][4]
#define NOTE_E5 notes[4][5]
#define NOTE_E6 notes[4][6]
#define NOTE_E7 notes[4][7]

#define NOTE_F0 notes[5][0]
#define NOTE_F1 notes[5][1]
#define NOTE_F2 notes[5][2]
#define NOTE_F3 notes[5][3]
#define NOTE_F4 notes[5][4]
#define NOTE_F5 notes[5][5]
#define NOTE_F6 notes[5][6]
#define NOTE_F7 notes[5][7]

#define NOTE_FSH0 notes[6][0]
#define NOTE_FSH1 notes[6][1]
#define NOTE_FSH2 notes[6][2]
#define NOTE_FSH3 notes[6][3]
#define NOTE_FSH4 notes[6][4]
#define NOTE_FSH5 notes[6][5]
#define NOTE_FSH6 notes[6][6]
#define NOTE_FSH7 notes[6][7]

#define NOTE_G0 notes[7][0]
#define NOTE_G1 notes[7][1]
#define NOTE_G2 notes[7][2]
#define NOTE_G3 notes[7][3]
#define NOTE_G4 notes[7][4]
#define NOTE_G5 notes[7][5]
#define NOTE_G6 notes[7][6]
#define NOTE_G7 notes[7][7]

#define NOTE_GSH0 notes[8][0]
#define NOTE_GSH1 notes[8][1]
#define NOTE_GSH2 notes[8][2]
#define NOTE_GSH3 notes[8][3]
#define NOTE_GSH4 notes[8][4]
#define NOTE_GSH5 notes[8][5]
#define NOTE_GSH6 notes[8][6]
#define NOTE_GSH7 notes[8][7]

#define NOTE_A0 notes[9][0]
#define NOTE_A1 notes[9][1]
#define NOTE_A2 notes[9][2]
#define NOTE_A3 notes[9][3]
#define NOTE_A4 notes[9][4]
#define NOTE_A5 notes[9][5]
#define NOTE_A6 notes[9][6]
#define NOTE_A7 notes[9][7]

#define NOTE_ASH0 notes[10][0]
#define NOTE_ASH1 notes[10][1]
#define NOTE_ASH2 notes[10][2]
#define NOTE_ASH3 notes[10][3]
#define NOTE_ASH4 notes[10][4]
#define NOTE_ASH5 notes[10][5]
#define NOTE_ASH6 notes[10][6]
#define NOTE_ASH7 notes[10][7]

#define NOTE_B0 notes[11][0]
#define NOTE_B1 notes[11][1]
#define NOTE_B2 notes[11][2]
#define NOTE_B3 notes[11][3]
#define NOTE_B4 notes[11][4]
#define NOTE_B5 notes[11][5]
#define NOTE_B6 notes[11][6]
#define NOTE_B7 notes[11][7]

#endif /* __IO_SOUND_H_ */
