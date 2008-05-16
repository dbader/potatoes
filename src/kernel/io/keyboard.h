/* $Id$
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
 * Header for the keyboard-handler.
 * Includes the keyboard-map (german).
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#define LSHIFT 0x2A
#define RSHIFT 0x36
#define ALT 0x38

char kb_map[]={
        0,      //empty
        0,      //0x01 Esc
        '1',    //0x02 
        '2',    //0x03
        '3',    //0x04
        '4',    //0x05
        '5',    //0x06
        '6',    //0x07
        '7',    //0x08
        '8',    //0x09
        '9',    //0x0A
        '0',    //0x0B
        '\341', //0x0C ß
        0,      //Ox0D ???
        '\b',   //0x0E
        '\t',   //0x0F
        'q',    //0x10
        'w',    //0x11
        'e',    //0x12
        'r',    //0x13
        't',    //0x14
        'z',    //0x15
        'u',    //0x16
        'i',    //0x17
        'o',    //0x18
        'p',    //0x19
        '\201',  //0x1A ü
        '+',    //0x1B
        '\n',   //0x1C
        0,      //0x1D Ctrl
        'a',    //0x1E
        's',    //0x1F
        'd',    //0x20
        'f',    //0x21
        'g',    //0x22
        'h',    //0x23
        'j',    //0x24
        'k',    //0x25
        'l',    //0x26
        '\224', //0x27 ö
        '\204', //0x28 ä
        0,      //0x29 ???
        0,      //0x2A LShift
        '#',    //0x2B
        'y',    //0x2C
        'x',    //0x2D
        'c',    //0x2E
        'v',    //0x2F
        'b',    //0x30
        'n',    //0x31
        'm',    //0x32
        ',',    //0x33
        '.',    //0x34
        '-',    //0x35
        0,      //0x36 RShift
        0,      //0x37
        0,      //0x38 Alt
        ' ',    //0x39
        0,      //0x3A Caps Lock
        0,      //0x3B
        0,      //0x3C
        0,      //0x3D
        0,      //0x3E
        0,      //0x3F
        0,      //0x40
        0,      //0x41
        0,      //0x42
        0,      //0x43
        0,      //0x44
        0,      //0x45 Num
        0,      //0x46 Scroll
        0,      //0x47 Pos1
        0,      //0x48 Up
        0,      //0x49 Page Up
        0,      //0x4A 
        0,      //0x4B Left
        0,      //0x4C
        0,      //0x4D Right
        0,      //0x4E
        0,      //0x4F End
        0,      //0x50 Down
        0,      //0x51 Page Down
        0,      //0x52 Insert
        0,      //0x53 Delete
        0,      //0x54 
        0,      //0x55
        0,      //0x56
        0,      //0x57
        0,      //0x58
        0,      //0x59
        0,      //0x5A
        0,      //0x5B Super
        0,      //0x5C
        0,      //0x5D Context menu
        0,      //0x5E
        0,      //0x5F
        0,      //0x60
        0,      //0x61
        0,      //0x62
        0,      //0x63
        0,      //0x64
        0,      //0x65
        0,      //0x66
        0,      //0x67
        0,      //0x68
        0,      //0x69
        0,      //0x6A
        0,      //0x6B
        0,      //0x6C
        0,      //0x6D
        0,      //0x6E
        0,      //0x6F
        0,      //0x70
        0,      //0x71
        0,      //0x72
        0,      //0x73
        0,      //0x74
        0,      //0x75
        0,      //0x76
        0,      //0x77
        0,      //0x78
        0,      //0x79
        0,      //0x7A
        0,      //0x7B
        0,      //0x7C
        0,      //0x7D
        0,      //0x7E
        0,      //0x7F
};

char kb_shift_map[]={
        0,      //empty
        0,      //0x01 Esc
        '!',    //0x02 
        '"',    //0x03
        '�',    //0x04
        '$',    //0x05
        '%',    //0x06
        '&',    //0x07
        '/',    //0x08
        '(',    //0x09
        ')',    //0x0A
        '=',    //0x0B
        '?',    //0x0C 
        0,      //Ox0D ???
        '\b',   //0x0E
        '\t',   //0x0F
        'Q',    //0x10
        'W',    //0x11
        'E',    //0x12
        'R',    //0x13
        'T',    //0x14
        'Z',    //0x15
        'U',    //0x16
        'I',    //0x17
        'O',    //0x18
        'P',    //0x19
        '\232', //0x1A Ü
        '*',    //0x1B
        '\n',   //0x1C
        0,      //0x1D Ctrl
        'A',    //0x1E
        'S',    //0x1F
        'D',    //0x20
        'F',    //0x21
        'G',    //0x22
        'H',    //0x23
        'J',    //0x24
        'K',    //0x25
        'L',    //0x26
        '\231', //0x27 Ö
        '\216', //0x28 Ä
        0,      //0x29 ???
        0,      //0x2A LShift
        '\'',    //0x2B
        'Y',    //0x2C
        'X',    //0x2D
        'C',    //0x2E
        'V',    //0x2F
        'B',    //0x30
        'N',    //0x31
        'M',    //0x32
        ';',    //0x33
        ':',    //0x34
        '_',    //0x35
        0,      //0x36 RShift
        0,      //0x37
        0,      //0x38 Alt
        ' ',    //0x39
        0,      //0x3A Caps Lock
        0,      //0x3B
        0,      //0x3C
        0,      //0x3D
        0,      //0x3E
        0,      //0x3F
        0,      //0x40
        0,      //0x41
        0,      //0x42
        0,      //0x43
        0,      //0x44
        0,      //0x45 Num
        0,      //0x46 Scroll
        0,      //0x47 Pos1
        0,      //0x48 Up
        0,      //0x49 Page up
        0,      //0x4A
        0,      //0x4B Left
        0,      //0x4C
        0,      //0x4D Right
        0,      //0x4E
        0,      //0x4F End
        0,      //0x50 Down
        0,      //0x51 Page down
        0,      //0x52 Insert
        0,      //0x53 Delete
        0,      //0x54
        0,      //0x55
        '>',    //0x56
        0,      //0x57
        0,      //0x58
        0,      //0x59
        0,      //0x5A
        0,      //0x5B Super
        0,      //0x5C
        0,      //0x5D Context menu
        0,      //0x5E
        0,      //0x5F
        0,      //0x60
        0,      //0x61
        0,      //0x62
        0,      //0x63
        0,      //0x64
        0,      //0x65
        0,      //0x66
        0,      //0x67
        0,      //0x68
        0,      //0x69
        0,      //0x6A
        0,      //0x6B
        0,      //0x6C
        0,      //0x6D
        0,      //0x6E
        0,      //0x6F
        0,      //0x70
        0,      //0x71
        0,      //0x72
        0,      //0x73
        0,      //0x74
        0,      //0x75
        0,      //0x76
        0,      //0x77
        0,      //0x78
        0,      //0x79
        0,      //0x7A
        0,      //0x7B
        0,      //0x7C
        0,      //0x7D
        0,      //0x7E
        0,      //0x7F
};

char kb_alt_map[]={
        0,      //empty
        0,      //0x01 Esc
        '\373',    //0x02 
        '\375',    //0x03
        '\374',    //0x04
        '\254',    //0x05
        '\253',    //0x06
        '\252',    //0x07
        '{',    //0x08
        '[',    //0x09
        ']',    //0x0A
        '}',    //0x0B
        '\\',   //0x0C
        0,      //Ox0D ???
        '\b',   //0x0E
        '\t',   //0x0F
        '@',    //0x10
        0,      //0x11
        '�',    //0x12 //FIXME: was '€'
        '\364', //0x13
        0,      //0x14
        0,      //0x15
        0,      //0x16
        0,      //0x17
        '\233', //0x18
        '\350', //0x19
        0,      //0x1A ü
        '~',    //0x1B
        '\n',   //0x1C
        0,      //0x1D Ctrl
        '\221', //0x1E
        '\225', //0x1F
        0,      //0x20
        0,      //0x21
        0,      //0x22
        0,      //0x23
        0,      //0x24
        0,      //0x25
        0,      //0x26
        0,      //0x27 ö
        0,      //0x28 ä
        0,      //0x29 ???
        0,      //0x2A LShift
        0,      //0x2B
        '\256', //0x2C
        '\257', //0x2D
        '\275', //0x2E
        0,      //0x2F
        0,      //0x30
        0,      //0x31
        0,      //0x32
        0,      //0x33
        0,      //0x34
        0,      //0x35
        0,      //0x36 RShift
        0,      //0x37
        0,      //0x38 Alt
        ' ',    //0x39
        0,      //0x3A Caps Lock
        0,      //0x3B
        0,      //0x3C
        0,      //0x3D
        0,      //0x3E
        0,      //0x3F
        0,      //0x40
        0,      //0x41
        0,      //0x42
        0,      //0x43
        0,      //0x44
        0,      //0x45 Num
        0,      //0x46 Scroll
        0,      //0x47 Pos1
        0,      //0x48 Up
        0,      //0x49 Page Up
        0,      //0x4A 
        0,      //0x4B Left
        0,      //0x4C
        0,      //0x4D Right
        0,      //0x4E
        0,      //0x4F End
        0,      //0x50 Down
        0,      //0x51 Page Down
        0,      //0x52 Insert
        0,      //0x53 Delete
        0,      //0x54 
        0,      //0x55
        0,      //0x56
        0,      //0x57
        0,      //0x58
        0,      //0x59
        0,      //0x5A
        0,      //0x5B Super
        0,      //0x5C
        0,      //0x5D Context menu
        0,      //0x5E
        0,      //0x5F
        0,      //0x60
        0,      //0x61
        0,      //0x62
        0,      //0x63
        0,      //0x64
        0,      //0x65
        0,      //0x66
        0,      //0x67
        0,      //0x68
        0,      //0x69
        0,      //0x6A
        0,      //0x6B
        0,      //0x6C
        0,      //0x6D
        0,      //0x6E
        0,      //0x6F
        0,      //0x70
        0,      //0x71
        0,      //0x72
        0,      //0x73
        0,      //0x74
        0,      //0x75
        0,      //0x76
        0,      //0x77
        0,      //0x78
        0,      //0x79
        0,      //0x7A
        0,      //0x7B
        0,      //0x7C
        0,      //0x7D
        0,      //0x7E
        0,      //0x7F
};

static char shift = 0;
static char alt = 0;
