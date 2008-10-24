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
 * Simple Potatoes/Etios EDitor.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/pm/syscalls_shared.h"
#include "../kernel/pm/pm_main.h"
#include "shell_utils.h"
#include "games.h"
#include "editor.h"
#include "../kernel/include/const.h"
#include "../kernel/include/string.h"

char* resize_buf(int newsize, char* buf)
{
        char* temp = buf;
        buf = _malloc(newsize);
        bzero(buf, newsize);
        memcpy(buf, temp, newsize / 2);
        _free(temp);
        return buf;
}

void speed() 
{
        int keyboard = _open("/dev/keyboard", 0, 0);
        int stdin = _open("/dev/stdin", 0, 0);
        int size = 2;
        char ch;
        int fd = _open(active_proc->name, 0, 0);
        char* str = _malloc(size);

        line *startline = _malloc(sizeof(line));
        startline->num_chars=0;
        startline->offset=0;
        startline->next = NULL;
        startline->prev = NULL;
        line * actualline = startline;

        bzero(str, size);
        int pos = 0;
        while (_read(fd, str+pos, 1) > 0 && str[pos]!='\0') {
                if(str[pos] == '\n' || str[pos] == '\t') {
                        NEXT_LINE
                } else {
                        actualline->num_chars++;
                }
                pos++;
                if(pos == size - 1) { //terminating \0
                        size*=2;
                        str = resize_buf(size, str);
                }
        }
        _close(fd);
        _printf(str);

        //flush stdin
        while (_read(stdin, &ch, sizeof(ch)) != 0) ;

        while (!keydown(ESCAPE, keyboard)) {
                if(pos == size - 1) { //terminating \0
                        size *= 2;
                        str = resize_buf(size, str);
                }
                ch = _fgetch(stdin);
                switch(ch){
                case '\a': 
                        break;
                case '\b':
                        if(actualline == startline && startline->num_chars == 0) { //nothing to erase
                                break;                                
                        }
                        if(str[pos - 1]=='\n' || str[pos - 1]=='\t') { //erase '\n' or '\t'
                                PREV_LINE
                        } else { //erase 1 character
                                actualline->num_chars--;
                                _printf("%c", ch);
                        }
                        str[--pos] = '\0';
                        if (pos == size / 4 - 1) {
                                size /= 2;
                                str = resize_buf(size, str);
                        }
                        break;
                default:
                        _printf("%c", ch);
                        str[pos] = ch;
                        if(ch == '\n' || ch == '\t') {
                                NEXT_LINE
                        } else {
                                actualline->num_chars++;
                        }
                        pos++;
                }
        }

        //flush stdin
        while (_read(stdin, &ch, sizeof(ch)) != 0) ;
        //DUMP_LINES
        _unlink(active_proc->name);
        //        fd = _open(active_proc->name, 0, 0);
        //        int fsize = _seek(fd, 0, SEEK_END);
        //        _close(fd);
        //        fd = _open(active_proc->name, 0, 0);
        //        char* nullstring = _malloc(fsize);
        //        bzero(nullstring, fsize);
        //        _free(nullstring);
        //        _write(fd, nullstring, fsize);
        //        _close(fd);

        fd = _open(active_proc->name, O_CREAT, 0);
        _write(fd, str, strlen(str));
        _free(startline);
        _free(str);
        _close(keyboard);
        _close(stdin);
        _close(fd);
        _exit(0);
}

void shell_cmd_speed(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: speed [path]\n");
                return;
        }

        char* path = shell_makepath(argv[1]);
        int fd = _open(path, O_CREAT, 0);
        if (fd < 0) {
                _printf("Failed to create regular file %s\n", path);
                return;
        }
        _close(fd);
        pm_create_thread(strdup(path), speed, 4096);
}
