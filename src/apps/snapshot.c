/* $Id: sn4psh0t.c 195 2008-08-29 22:54:26Z dtraytel $
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
 * Programm to make and view screenshots.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 195 $
 */
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/pm/syscalls_shared.h"
#include "../kernel/include/const.h"
#include "../kernel/include/string.h"
#include "../kernel/include/stdlib.h"
#include "../kernel/io/io.h"
#include "../kernel/io/io_virtual.h"
#include "../kernel/pm/pm_main.h"
#include "games.h"

char snap_buffer[2*25*80];

void snapshot()
{
        _close(_open("/snapshots/", O_CREAT, 0));

        char* str = _malloc(50);
        memset(str, 0, 50);
        strcpy(str, "/snapshots/");

        int stdin = _open("/dev/stdin", 0, 0);

        _printf("Please enter the snapshot file name and hit 'enter': ");
        _fgets(str + strlen(str), 50 - strlen(str), stdin);
        *(str + strlen(str) - 1) = 0; //killing the line feed

        int fd = _open(str, O_CREAT, 0);
        _write(fd, snap_buffer, 2*25*80);

        _free(str);
        _close(stdin);
        _close(fd);
        _exit(0);
}

extern int STDIN;
extern int STDOUT;
char* shell_makepath(char *path);

void shell_cmd_snapshot(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: view [path]\n", STDOUT);
                return;
        }
        int fd = _open(shell_makepath(argv[1]), 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        int keyboard = _open("/dev/keyboard", 0, 0);
        get_active_virt_monitor()->disable_refresh = TRUE;
        _read(fd, (void*)VGA_DISPLAY, 2*25*80);

        while (!keydown(ESCAPE, keyboard)) ;

        get_active_virt_monitor()->disable_refresh = FALSE;

        char ch;
        while (_read(STDIN, &ch, sizeof(ch)) != 0) ;

        _close(keyboard);
        _close(fd);
}

void make_snapshot()
{
        memcpy(snap_buffer, (void*)VGA_DISPLAY, 2*25*80);
        pm_create_thread("SNAPSHOT", snapshot, 4096);
}
