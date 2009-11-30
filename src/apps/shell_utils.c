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
 * Shell utility functions.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../kernel/include/types.h"
#include "../kernel/include/const.h"
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/include/string.h"
#include "../kernel/include/stdarg.h"
#include "../kernel/include/stdio.h"
#include "shell_main.h"

/** The shell's STDIN file descriptor. Used by all internal commands. */
int STDIN = -1;

/** The shell's STDOUT file descriptor. Used by all internal commands. */
int STDOUT = -1;

/**
 * Writes a character to the given file.
 *
 * @param ch the character to write
 * @param fd the file descriptor
 * @return the number of bytes written, -1 on error
 */
int _fputch(char ch, int fd)
{
        return _write(fd, &ch, sizeof(ch));
}

extern void halt();

/**
 * Waits until a character from the given file could be read and returns it.
 *
 * @param fd the file descriptor
 * @return the character that was read
 */
int _fgetch(int fd)
{
        char ch;
        while (_read(fd, &ch, sizeof(ch)) == 0)
                halt();
        return ch;
}

/**
 * Reads a string from a file descriptor into a buffer.
 *
 * @param s the string buffer
 * @param n the maximum number of bytes to read (ie the buffer size)
 * @param fd the file descriptor
 * @return the string buffer
 */
char* _fgets(char *s, int n, int fd)
{
        // FIXME: This is ugly.

        char *start = s;
        int count = 0;
        char ch = 0;
        while ((n-- > 0) && (ch != '\n')) {
                ch = _fgetch(fd);

                // Tab
                if (ch == '\t') {
                        *s++ = ch;
                        break;
                }

                // Handle backspace
                if (ch != '\b') {
                        *s++ = ch;
                        count++;
                        _fputch(ch, STDOUT);
                } else {
                        if (count > 0) {
                                *s-- = '\0';
                                count--;
                                _fputch(ch, STDOUT);
                        }
                }

        }

        return s;

}

/**
 * Writes a string into a given file.
 *
 * @param s the string to write
 * @param fd the file descriptor
 * @return the number of bytes written, -1 on error
 */
int _fputs(char *s, int fd)
{
        int count = strlen(s);
        return _write(fd, s, count);
}

/**
 * Prints formatted output to STDOUT. @see printf
 * This exists as a stub to ease the separation of the shell
 * from the kernel code (as of now, the shell could simply call the kernel printf).
 *
 * TODO: refactor format logic into vprintf()
 */
void _printf(char *fmt, ...) //TODO: @Daniel: redundant. --> better solution possible?
{
        char buf[255];
        va_list arg_list;
        va_start(arg_list, fmt);

        vsnprintf(buf, sizeof(buf), fmt, arg_list);
        _write(STDOUT, buf, sizeof(buf));

        va_end(arg_list);
}

/**
 * Makes a given path absolute if needed. shell_makepath checks for a leading slash
 * in path to decide whether a given path is already absolute. If the path is not absolute
 * it will be appended to the current working directory.
 * Calling this will invalidate the last result.
 *
 * @param path the path to make absolute
 * @return the absolute path. This pointer is only valid until the next call to
 * 		   shell_makepath().
 */
char* shell_makepath(char *path)
{
        if (path[0] == '/') {
                strcpy(path_buf, path);
                // absolute path
                return path_buf;
        } else {
                // Relative path.
                strcpy(path_buf, cwd);

                // Append trailing slash
                if (path_buf[strlen(path_buf)-1] != '/')
                        strcat(path_buf, "/");

                strcat(path_buf, path);
                path_buf[strlen(path_buf)] = '\0';
                return path_buf;
        }
}
