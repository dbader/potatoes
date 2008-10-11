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
 * Reads a file and dumps its contents into a C byte array.
 * Example:
 *      - contents of input.bin: "Hello, World. This is a test."
 *      - then run "bin2c input.bin > test.c"
 *      - you get:
 * @code
 *               unsigned char data[] = {
 *                               0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f,
 *                               0x72, 0x6c, 0x64, 0x21, 0x20, 0x54, 0x68, 0x69, 0x73,
 *                               0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65, 0x73,
 *                               0x74, 0x2e, 0x0a
 *               }
 * @endcode
 *
 * Build via "gcc bin2c.c -o bin2c" (the makefile also has a target called bin2c)
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include <stdio.h>
#include <stdlib.h>

#define VARHDR "/* %s, %d bytes */\nunsigned char data[] = {\n"
#define VARFMT "0x%.2x"
#define VAREND "\n}\n"
#define VARTAB "        "
#define ITEMSPERLINE 15

int main(int argc, char **argv)
{
        if (argc < 2) {
                printf("bin2c (c) Daniel Bader 2008\nUsage: bin2c [input_file]\n");
                exit(1);
        }

        FILE *in = fopen(argv[1], "rb");

        if (!in) {
                printf("error opening input file %s\n", argv[1]);
                exit(1);
        }

        fseek(in, 0, SEEK_END);
        int size = ftell(in);
        fseek(in, 0, SEEK_SET);

        unsigned char *buf = (unsigned char*) malloc(size);
        fread(buf, 1, size, in);

        printf(VARHDR, argv[1], size);
        printf(VARTAB);

        int i;
        for (i = 0; i < size; i++) {
                if (i > 0 && i % ITEMSPERLINE == 0)
                        printf("\n%s", VARTAB);
                printf(VARFMT, buf[i]);
                if (i != size - 1)
                        printf(", ");
        }

        printf(VAREND);

        free(buf);
        fclose(in);

        return EXIT_SUCCESS;
}
