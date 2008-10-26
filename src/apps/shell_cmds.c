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
 * Implementations of the shell commands.
 * 
 * TODO: nice to have:
 *
 * proper tab completion
 * repeat last command
 * globbing
 * rm
 * mv
 * kill
 * df
 * exit
 * exec
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../kernel/io/io.h"
#include "../kernel/include/string.h"
#include "../kernel/include/const.h"
#include "../kernel/include/limits.h"
#include "../kernel/include/stdlib.h"
#include "../kernel/include/stdio.h"
#include "../kernel/fs/fs_types.h"
#include "../kernel/fs/fs_const.h"
#include "../kernel/pm/syscalls_shared.h"
#include "../kernel/pm/syscalls_cli.h"
#include "games.h"
#include "apps.h"
#include "shell_main.h"
#include "shell_utils.h"

/**
 * A simple test command that prints all of its arguments to the screen.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_test(int argc, char *argv[])
{
        for (int i = 0; i < argc; i++)
                _printf("argv[%d] = '%s'\n", i, argv[i]);
}

/**
 * Lists all available arguments.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_cmdlist(int argc, char *argv[])
{
        _printf("Available commands are:\n", STDOUT);

        shell_cmd_t *cmd;
        int i = 0;

        while ((cmd = &shell_cmds[i++])->cmd != NULL) {
                _printf("\t%s\t\t- %s\n", cmd->name, cmd->desc, STDOUT);
        }
}

/**
 * Prints its arguments to the screen.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_echo(int argc, char *argv[])
{
        for (int i = 1; i < argc; i++) {
                _printf("%s ", argv[i]);
        }

        _printf("\n");
}

/**
 * Lists directory contents. 
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_ls(int argc, char *argv[])
{
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));

        char path[255];

        int fd = -1;
        if (argc < 2) {
                fd = _open(cwd, 0, 0);   // current directory
                strncpy(path, cwd, sizeof(path));
        } else {
                strncpy(path, shell_makepath(argv[1]), sizeof(path));
                fd = _open(path, 0, 0);
        }

        if (path[strlen(path)-1] != '/')
                strcat(path, "/");

        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        _read(fd, directory, sizeof(directory));

        int total = 0;

        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++) {
                if (directory[i].inode != NULL) {
                        stat stat_buf;                       
                        char abs_path[255];
                        char time[25];

                        strncpy(abs_path, path, sizeof(abs_path));
                        strcat(abs_path, directory[i].name);

                        if (_stat(abs_path, &stat_buf) == 0) {
                                total += stat_buf.size;
                                _printf("%d\t%s %s", 
                                                stat_buf.size,
                                                time2str(stat_buf.modify_ts, time),
                                                directory[i].name);
                                if (stat_buf.mode == DIRECTORY) {
                                        _printf("/");
                                }

                                _printf("\n");
                        } else {
                                _printf("%s ERROR: stat() failed.\n", directory[i].name);
                        }
                }
        }

        _printf("total %d bytes\n", total);
        _close(fd);
}

/**
 * Creates an empty file.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

/**
 * Creates a new directory.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

/**
 * Prints file contents.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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
        
        char buf[512];
        int read;
        do {
                read = _read(fd, buf, sizeof(buf));
                _write(STDOUT, buf, read);
        } while (read == sizeof(buf));

        _fputch('\n', STDOUT);
        _close(fd);
}

/**
 * Writes text into a file.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

                if ((i+1) != argc) {
                        count += _write(fd, " ", 1);
                }
        }

        if (count < 0) {
                _printf("%s: %s: Cannot write\n", argv[0], argv[1]);
        } else {
                _printf("wrote %d bytes.\n", count);//_seek(fd, 0, SEEK_CUR));
        }

        _close(fd);
}

/**
 * Changes the current directory.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

/**
 * Clears the screen.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

/**
 * Flushes all fs memory buffers to disk.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_sync(int argc, char *argv[])
{
        fs_shutdown();
        fs_init();
}

extern void mem_dump();

/**
 * Dumps memory stats and allocated blocks.
 * @see mem_dump
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_memdump(int argc, char *argv[])
{
        mem_dump();
}

/**
 * Prints the current working directory.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_pwd(int argc, char *argv[])
{
        _printf("%s\n", cwd);
}

/**
 * Copies a file.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
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

        char buf[512];
        int read;

        do {
                read = _read(src_fd, buf, sizeof(buf));
                _write(target_fd, buf, read);

        } while (read == sizeof(buf));

        _close(src_fd);
        _close(target_fd);
}

extern void pm_dump();

/**
 * Prints info about running processes.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_ps(int argc, char *argv[])
{
        pm_dump();
}

void reset_bf();

/**
 * Interface to the brainfuck interpreter device.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_bf(int argc, char *argv[])
{
        int bf = _open("/dev/brainfuck", 0, 0);
        if (!strcmp(argv[1], "-i")) {
                _write(bf, argv[2], strlen(argv[2]));
        } else {
                reset_bf();
                int fd = _open(shell_makepath(argv[1]), 0, 0);
                int temp = get_ticks();
                if (fd < 0) {
                        _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                        return;
                }

                char buf[512];
                int read;
                do {
                        read = _read(fd, buf, sizeof(buf));
                        _write(bf, buf, read);
                } while (read == sizeof(buf));

                temp = get_ticks() - temp;
                (temp < 0) ? temp += UINT32_MAX : temp;
                _printf("time elapsed: %d ticks\n", temp);
                _close(fd);
                reset_bf();
        }
        _close(bf);
}

/**
 * Exits the shell.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_exit(int argc, char *argv[])
{
        _printf("Bye.\n");
        _exit(0);
}

/**
 * Prints the current time and date.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_date(int argc, char *argv[])
{
        int fd = _open("/dev/clock", 0, 0);

        char buf[100];
        _read(fd, buf, sizeof(buf));
        _close(fd);

        _printf("%s\n", buf);
}

/**
 * Removes a file.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_rm(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: rm [path]\n");
                return;
        }

        int success = _unlink(shell_makepath(argv[1]));

        if (success == -1) {
                _printf("%s: cannot remove '%s'\n", argv[0], argv[1]);
        }
}

/**
 * Removes a file.
 * 
 * @param argc the number of argument strings in argv
 * @param argv the argument vector. Contains all arguments of the command.
 */
void shell_cmd_exec(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: run [path]\n");
                return;
        }

        int fd = _open(shell_makepath(argv[1]), 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }

        char* str = _malloc(81);
        bzero(str, 81);
        char* temp = str;
        while(_read(fd, temp++, 1) > 0 && (temp - str) < 80) {
                //_printf(str);
                if(*(temp - 1) == '\n') {
                        //_printf(str);
                        shell_handle_command(str);
                        bzero(str, 81);
                        temp = str;
                }
        }

        //FIXME: HACK
        if(*(str+strlen(str)) != '\n') {
                *(str+strlen(str)) = '\n';
        }

        shell_handle_command(str);
        _close(fd);
        _free(str);
}

void make_snapshot();

/** 
 * The shell command table. Every shell command must be registered here
 * to be accessible. */
struct shell_cmd_t shell_cmds[] = {
                {"cmdlist",     shell_cmd_cmdlist,      "List available commands\n"},
                
                {"test",        shell_cmd_test,         "Test argument parsing"},
                {"sync",        shell_cmd_sync,         "Writes the filesystem to disk"},
                {"exec",        shell_cmd_exec,         "Executes a \"batch\" file"},
                {"exit",        shell_cmd_exit,         "Quit the shell\n"},
                
                {"memdump",     shell_cmd_memdump,      "Dump allocated blocks"},
                {"ps",          shell_cmd_ps,           "List processes\n"},
                
                {"echo",        shell_cmd_echo,         "Print text to STDOUT"},
                {"clear",       shell_cmd_clear,        "Clear the screen"},
                {"view",        shell_cmd_snapshot,     "Displays an etiOS snapshot"},
                {"date",        shell_cmd_date,         "Display date and time\n"},
                
                {"pwd",         shell_cmd_pwd,          "Print working directory"},
                {"ls",          shell_cmd_ls,           "List directory"},
                {"cd",          shell_cmd_cd,           "Change directory"},
                {"mkdir",       shell_cmd_mkdir,        "Create directory"},
                {"touch",       shell_cmd_touch,        "Create regular file"},
                {"cp",          shell_cmd_cp,           "Copy files"},
                {"rm",          shell_cmd_rm,           "Removes a file\n"},
                
                {"cat",         shell_cmd_cat,          "Print file contents"},
                {"write",       shell_cmd_write,        "Write text to file\n"},

                {"speed",       shell_cmd_speed,        "Simple potatoes/etios editor"},
                {"bf",          shell_cmd_bf,           "Brainfuck interpreter"},
                {"pong",        shell_cmd_pong,         "A classic video game"},
                {"snake",       shell_cmd_snake,        "Another classic video game"},
                {"",            NULL,                   ""} // The Terminator
};

