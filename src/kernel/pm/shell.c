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
 * etiOS shell
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "syscalls_shared.h"
#include "syscalls_cli.h"
#include "../io/io.h"
#include "../include/string.h"
#include "../include/const.h"
#include "../include/limits.h"
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../fs/fs_types.h"

#include "../../apps/games.h"

int STDIN = -1;
int STDOUT = -1;

int _fputch(char ch, int fd)
{
        return _write(fd, &ch, sizeof(ch));
}

int _fgetch(int fd)
{
        char ch;
        while (_read(fd, &ch, sizeof(ch)) == 0)
                halt();
        return ch;
}

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

int _fputs(char *s, int fd)
{
        int count = strlen(s);
        return _write(fd, s, count);
}

void _printf(char *fmt, ...)
{
        if (fmt == NULL)
                return;

        char **arg = &fmt + 1;
        char ch;
        int character;
        char buf[40];

        while ((ch = *fmt++) != '\0')
                if (ch == '%') {
                        ch = *fmt++;
                        switch (ch) {
                        case '%': // print '%'
                                _fputch(ch, STDOUT);
                                break;
                        case 'i': // signed integer
                        case 'd':
                                _fputs(itoa((sint32)*arg++, buf, 10), STDOUT);
                                break;
                        case 'u': // unsigned integer
                                _fputs(itoa((uint32)*arg++, buf, 10), STDOUT);
                                break;
                        case 'o': // octal
                                _fputs(itoa((uint32)*arg++, buf, 8), STDOUT);
                                break;
                        case 'b': // binary
                                _fputs(itoa((uint32)*arg++, buf, 2), STDOUT);
                                break;
                        case 'c': // character
                                /* This is a bit peculiar but needed to shut up the
                                 * "cast from pointer to integer of different size"
                                 * compiler warning.
                                 * Code was: putchar((char)*arg++);
                                 */
                                character = (int) * arg++;
                                _fputch((char)character, STDOUT);
                                break;
                        case 's': // string
                                if (*arg != NULL) {
                                        while ((ch = *(*arg)++) != '\0')
                                                _fputch(ch, STDOUT);
                                } else {
                                        _fputs("(null)", STDOUT);
                                }
                                *arg++;
                                break;
                        case 'x': // hexadecimal integer
                        case 'p': // pointer
                                _fputs(itoa((uint32)*arg++, buf, 16), STDOUT);
                                break;
                        }
                } else
                        _fputch(ch, STDOUT);
}

typedef void (*shell_cmd_func)(int argc, char *argv[]);

/** A single shell commmand. */
typedef struct shell_cmd_t {
        char name[16];
        shell_cmd_func cmd;
        char desc[100];
} shell_cmd_t;

struct shell_cmd_t shell_cmds[]; // Forward declaration.

/** The current working directory. All relative paths are relative to this. */
char cwd[255];

/** A buffer for shell_makepath() */
char path_buf[sizeof(cwd)];

/**
 * Makes a given path absolute if needed. shell_makepath checks for a leading slash
 * in path to decide whether a given path is already absolute. If the path is not absolute
 * it will be appended to the current working directory.
 * Calling this will invalidate the last result.
 */
char* shell_makepath(char *path)
{
        strcpy(path_buf, cwd);

        if (path[0] == '/') {
                // absolute path
                return path;
        } else {
                // Relative path.

                // Append trailing slash
                if (path_buf[strlen(path_buf)-1] != '/')
                        strcat(path_buf, "/");

                strcat(path_buf, path);
                path_buf[strlen(path_buf)] = '\0';
                return path_buf;
        }
}

// THE COMMANDS

void shell_cmd_test(int argc, char *argv[])
{
        for (int i = 0; i < argc; i++)
                _printf("argv[%d] = '%s'\n", i, argv[i]);
}

void shell_cmd_cmdlist(int argc, char *argv[])
{
        _printf("Available commands are:\n", STDOUT);

        shell_cmd_t *cmd;
        int i = 0;

        // I dont think this is very cute.
        while ((cmd = &shell_cmds[i++])->cmd != NULL)
                _printf("\t%s\t\t- %s\n", cmd->name, cmd->desc, STDOUT);
}

void shell_cmd_echo(int argc, char *argv[])
{
        for (int i = 1; i < argc; i++)
                _printf("%s ", argv[i]);
        _printf("\n");
}


void shell_cmd_ls(int argc, char *argv[])
{
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));

        int fd = -1;
        if (argc < 2)
                fd = _open(cwd, 0, 0);   // current directory
        else
                fd = _open(shell_makepath(argv[1]), 0, 0);

        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        int num_bytes = _read(fd, directory, sizeof(directory));
        //_printf("%d bytes read.\n", num_bytes);

        /*if (num_bytes != sizeof(directory)) {
                _close(fd);
                _printf("%s: Error reading directory\n", argv[0]);
                return;
        }*/

        int i = 0;
        while (directory[i].inode != 0) {
                _printf("%s\n", directory[i].name);
                i++;
        }

        _close(fd);
}

void shell_cmd_touch(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: touch [file]\n");
                return;
        }

        char *path = shell_makepath(argv[1]);
        int fd = _open(path, O_CREAT, 0);

        if (fd >= 0)
                _printf("Created regular file %s\n", path);
        else
                _printf("Failed to create regular file %s\n", path);

        _close(fd);
}

void shell_cmd_mkdir(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: mkdir [path]\n", STDOUT);
                return;
        }

        char *path = shell_makepath(argv[1]);

        // Append a trailing slash to let open() know we want to create a directory.
        if (path[strlen(path)-1] != '/')
                strcat(path, "/");

        int fd = _open(path, O_CREAT, 0);
        if (fd >= 0)
                _printf("Created directory %s\n", argv[1]);
        else
                _printf("Failed to create directory %s\n", argv[1]);

        _close(fd);
}

void shell_cmd_cat(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: cat [file]\n");
                return;
        }

        int fd = _open(shell_makepath(argv[1]), 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        char ch;
        while (_read(fd, &ch, sizeof(ch)) > 0)
                _fputch(ch, STDOUT);

        _fputch('\n', STDOUT);
        _close(fd);
}

void shell_cmd_write(int argc, char *argv[])
{
        if (argc < 3) {
                _printf("Usage: write [file] [text]\n");
                return;
        }

        int fd = _open(shell_makepath(argv[1]), 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        int count = 0;
        for (int i = 2; i < argc; i++) {
                count += _write(fd, argv[i], strlen(argv[i]));
                count += _write(fd, " ", 1);
        }

        count += _write(fd, "", 1); // append \0
        _printf("wrote %d bytes.\n", count);//_seek(fd, 0, SEEK_CUR));
        _close(fd);
}

void shell_cmd_cd(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: cd [path]\n");
                return;
        }

        /*
         * TODO:
         * - handle cd . and cd .. (ideally the fs includes these as dummy directories)
         */

        char *new_dir = shell_makepath(argv[1]);

        int fd = _open(new_dir, 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        // It exists, change the cwd.
        strcpy(cwd, new_dir);

        _close(fd);
}

void shell_cmd_clear(int argc, char *argv[])
{
        for (int i = 0; i < 24 * 80; i++)
                _fputch(' ', STDOUT);
        for (int i = 0; i < 24 * 80; i++)
                _fputch('\b', STDOUT);
}

extern void fs_shutdown();
extern void fs_init();
extern bool create_fs();

void shell_cmd_sync(int argc, char *argv[]) //TODO: @Daniel: strange intention... every thing will be reset
{
        fs_shutdown();
//        create_fs();
//        fs_shutdown();
        fs_init();
}

extern void mem_dump();
void shell_cmd_memdump(int argc, char *argv[])
{
//        printf("%{mm:} malloc testing - 0x%x\n", LIGHTBLUE, mallocn(1,"test1"));
//        printf("%{mm:} malloc testing - 0x%x\n", LIGHTBLUE, mallocn(2,"test2"));
//        printf("%{mm:} malloc testing - 0x%x\n", LIGHTBLUE, mallocn(3,"test3"));
        mem_dump();
}

void shell_cmd_pwd(int argc, char *argv[])
{
        _printf("%s\n", cwd);
}

void shell_cmd_cp(int argc, char *argv[])
{
        if (argc < 3) {
                _printf("Usage: cp [source] [target]\n");
                return;
        }

        int src_fd = _open(shell_makepath(argv[1]), 0, 0);
        if (src_fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        int target_fd = _open(shell_makepath(argv[2]), O_CREAT, 0);
        if (src_fd < 0) {
                _printf("%s: %s: Could not create target file\n", argv[0], argv[2]);
                _close(src_fd);
                return;
        }

        char ch;
        while (_read(src_fd, &ch, sizeof(ch)) != 0)
                _fputch(ch, target_fd);

        _close(src_fd);
        _close(target_fd);
}

extern void pm_dump();
void shell_cmd_ps(int argc, char *argv[])
{
        pm_dump();
}

void reset_bf();
void shell_cmd_bf(int argc, char *argv[])
{
        if (!strcmp(argv[1], "-i")) {
                _write(5, argv[2], strlen(argv[2]));
        } else {
                reset_bf();
                int fd = _open(shell_makepath(argv[1]), 0, 0);
                int temp = get_ticks();
                if (fd < 0) {
                        _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                        return;
                }
                char ch;
                while (_read(fd, &ch, sizeof(ch)) != 0) {
                        _fputch(ch, 5);
                }
                temp = get_ticks() - temp;
                (temp < 0) ? temp += UINT32_MAX : temp;
                _printf("time elapsed: %d ticks\n", temp);
                _close(fd);
                reset_bf();
        }
}

void shell_cmd_exit(int argc, char *argv[])
{
        _printf("Bye.\n");
        _exit(0);
}

void shell_cmd_date(int argc, char *argv[])
{
        int fd = _open("/dev/clock", 0, 0);

        char buf[100];
        _read(fd, buf, sizeof(buf));
        _close(fd);

        _printf("%s\n", buf);
}

// TODO: nice to have:
/*
 * tab completion
 * repeat last command
 * globbing
 * rm
 * mv
 * kill
 * df
 * exit
 * exec
 */


struct shell_cmd_t shell_cmds[] = {
        {"test",        shell_cmd_test,         "Test argument parsing"},
        {"cmdlist",     shell_cmd_cmdlist,      "List available commands"},
        {"echo",        shell_cmd_echo,         "Print text to STDOUT"},
        {"ls",          shell_cmd_ls,           "List directory"},
        {"touch",       shell_cmd_touch,        "Create regular file"},
        {"mkdir",       shell_cmd_mkdir,        "Create directory"},
        {"cat",         shell_cmd_cat,          "Print file contents"},
        {"write",       shell_cmd_write,        "Write text to file"},
        {"cd",          shell_cmd_cd,           "Change directory"},
        {"clear",       shell_cmd_clear,        "Clear the screen"},
        {"sync",        shell_cmd_sync,         "Writes the filesystem to disk"},
        {"memdump",     shell_cmd_memdump,      "Dump allocated blocks"},
        {"pwd",         shell_cmd_pwd,          "Print working directory"},
        {"cp",          shell_cmd_cp,           "Copy files"},
        {"ps",          shell_cmd_ps,           "List processes"},
        {"exit",        shell_cmd_exit,         "Quit the shell"},
        {"bf",          shell_cmd_bf,           "Brainfuck interpreter"},
        {"pong",        shell_cmd_pong,         "A classic video game"},
        {"snake",       shell_cmd_snake,        "Another classic video game"},
        {"date",        shell_cmd_date,         "Display date and time"},
        {"",            NULL,                   ""} // The Terminator
};

#define NUM_SHELL_COMMANDS (sizeof(shell_cmds) / sizeof(shell_cmd_t)) - 1

void shell_handle_command(char *cmd)
{
        // Ignore returns
        if (strlen(cmd) == 1)
                return;

        // Kill the trailing line feed.
        cmd[strlen(cmd)-1] = '\0';

        // Parsing setup
        char delim[] = " ";
        char *tok;
        char *copy = strdup(cmd); // uses kernel malloc
        //printf("malloc by strdup (copy): 0x%x\n", copy);
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
                //printf("malloc by strdup (argv[i]): 0x%x\n", argv[argc-1]);
        }
        //printf("_free(copy=%s): 0x%x\n", copy, copy);
        _free(copy);

        shell_cmd_t *command = NULL;

        // Find the command
        for (int i = 0; i < NUM_SHELL_COMMANDS; i++)
                if (strcmp(argv[0], shell_cmds[i].name) == 0) {
                        command = &shell_cmds[i];
                        break;
                }

        // Execute it if possible
        if (command)
                command->cmd(argc, argv);
        else
                _printf("- shell: %s: command not found\n", argv[0]);

        for (int i = 0; i < argc; i++) {
                //printf("_free(argv[%d]=%s): 0x%x\n", i, argv[i], argv[i]);
                _free(argv[i]);
        }
}

void shell_autocomplete(char *partial, int n)
{
        partial[strlen(partial)-1] = '\0';
        //_printf("\ncomplete: %s", partial);

        shell_cmd_t *cmd;
        int i = 0;
        char cmd_name[16];

        if (strlen(partial) > 15)
                return;

        while ((cmd = &shell_cmds[i++])->cmd != NULL) {
                memset(cmd_name, 0, sizeof(cmd_name));
                strncpy(cmd_name, cmd->name, strlen(partial));
                //_printf("%s vs %s = %d\n", partial, cmd_name, strcmp(partial, cmd_name));
                if (strcmp(partial, cmd_name) == 0) {
                        //_printf("best match is %s\n", cmd->name);
                        _fputs(cmd->name + strlen(partial), STDOUT);
                        strcat(partial, cmd->name + strlen(partial));
                        return;
                }
        }

}

void shell_main()
{
        STDIN = _open("/dev/stdin", 0, 0);
        STDOUT = _open("/dev/stdout", 0, 0);

        _printf("Welcome to etiOS!\n");
        _printf("Try \"cmdlist\" for a list of commands.\n\n");

        strcpy(cwd, "/");

        // Prompt
        while (1) {
                _printf("%s$ ", cwd);
                char cmd[512];
                memset(cmd, 0, sizeof(cmd));

                while ((cmd[strlen(cmd)-1] != '\n')) {
                        _fgets(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), STDIN);

                        if (cmd[strlen(cmd)-1] == '\t') {
                                shell_autocomplete(cmd, sizeof(cmd));
                        }
                }

                shell_handle_command(cmd);
        }

        _close(STDIN);
        _close(STDOUT);
        _exit(0);
}
