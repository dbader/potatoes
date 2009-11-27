/* $Id: snapshot.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * Programm to make and view screenshots.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
