/* $Id: shell_main.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * etiOS shell main header file.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
