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
 * etiOS shell main header file.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */
 
#ifndef SHELL_MAIN_H_
#define SHELL_MAIN_H_

void new_shell();

/** A shell command function pointer. Entry point of all shell commands. */
typedef void (*shell_cmd_func)(int argc, char *argv[]);

/** A single shell commmand. */
typedef struct shell_cmd_t {
        char name[16];
        shell_cmd_func cmd;
        char desc[100];
} shell_cmd_t;

extern struct shell_cmd_t shell_cmds[]; // Forward declaration.

/** The current working directory. All relative paths are relative to this. */
extern char cwd[255];

/** A buffer for shell_makepath() */
extern char path_buf[sizeof(cwd)];

void shell_handle_command(char *cmd);

#endif /*SHELL_MAIN_H_*/
