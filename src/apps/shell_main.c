/* $Id: shell_main.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * etiOS shell entry point and main source file.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#include "../kernel/include/string.h"
#include "../kernel/include/const.h"
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/pm/pm_main.h"
#include "shell_main.h"
#include "shell_utils.h"

/** The current working directory. All relative paths are relative to this. */
char cwd[255];

/** A buffer for shell_makepath() */
char path_buf[sizeof(cwd)];

/**
 * Parses and executes a given shell command.
 *
 * @param cmd string to containing the shell command
 */
void shell_handle_command(char *cmd)
{
        // Ignore returns
        if (strlen(cmd) == 1) {
                return;
        }

        // Kill the trailing line feed.
        cmd[strlen(cmd)-1] = '\0';

        // Parsing setup
        char delim[] = " ";
        char *tok;
        char *copy = strdup(cmd); // uses kernel malloc
        char *work_copy = copy;

        // For the command
        int argc = 0;
        char *argv[16];

        while ((tok = strsep(&work_copy, delim)) != NULL) {
                if (argc >= (sizeof(argv) / sizeof(char*))) {
                        _printf("- shell: argument overflow. Last argument: %s\n", argv[argc-1]);
                        break;
                }
                argv[argc++] = strdup(tok);
        }
        _free(copy);

        shell_cmd_t *command = NULL;

        // Find the command
        for (int i = 0; shell_cmds[i].cmd != NULL; i++) {
                if (strcmp(argv[0], shell_cmds[i].name) == 0) {
                        command = &shell_cmds[i];
                        break;
                }
        }

        // Execute it if possible
        if (command) {
                command->cmd(argc, argv);
        } else {
                _printf("- shell: %s: command not found\n", argv[0]);
        }

        for (int i = 0; i < argc; i++) {
                _free(argv[i]);
        }
}

/**
 * Attempts to autocomplete a given partial shell command.
 *
 * @param partial the partial command string. Will be extended by the first
 *                valid completion found.
 */
void shell_autocomplete(char *partial)
{
        partial[strlen(partial)-1] = '\0';

        shell_cmd_t *cmd;
        int i = 0;
        char cmd_name[16];

        if (strlen(partial) > 15)
                return;

        while ((cmd = &shell_cmds[i++])->cmd != NULL) {
                memset(cmd_name, 0, sizeof(cmd_name));
                strncpy(cmd_name, cmd->name, strlen(partial));

                if (strcmp(partial, cmd_name) == 0) {
                        _fputs(cmd->name + strlen(partial), STDOUT);
                        strcat(partial, cmd->name + strlen(partial));
                        return;
                }
        }

}

/**
 * The shell entry point and main loop. This is where the shell prompt gets
 * displayed and user typed commands are dispatched.
 */
void shell_main()
{
        STDIN = _open("/dev/stdin", 0, 0);
        STDOUT = _open("/dev/stdout", 0, 0);

        _printf("#{LGR}Welcome to #{RED}P#{GRE}O#{BLU}T#{RED}A#{GRE}T#{BLU}O#{RED}E#{GRE}S!##\n");
        _printf("#{LGR}Try \"cmdlist\" for a list of commands.##\n\n");

        strcpy(cwd, "/");

        // Prompt
        while (1) {
                STDIN = _open("/dev/stdin", 0, 0);
                STDOUT = _open("/dev/stdout", 0, 0);
                _printf("#{YEL}%s#{LBL}$## ", cwd);
                char cmd[512];
                memset(cmd, 0, sizeof(cmd));

                while (cmd[strlen(cmd)-1] != '\n') {
                        _fgets(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), STDIN);

                        if (cmd[strlen(cmd)-1] == '\t') {
                                shell_autocomplete(cmd);
                        }
                }
                _close(STDIN);
                _close(STDOUT);
                shell_handle_command(cmd);
        }
        _exit(0);
}

/**
 * Starts a new shell in an own thread.
 */
void new_shell()
{
        static int shellnum = 0;
        char* name = _malloc(12);
        char* temp = _malloc(12);
        strcpy(name, "SHELL ");
        strncat(name, itoa(shellnum++, temp, 10), 5);
        pm_create_thread(name, shell_main, 4096);
        _free(name);
        _free(temp);
}
