#include "syscalls_shared.h"
#include "syscalls_cli.h"
#include "../io/io.h"
#include "../include/string.h"
#include "../include/const.h"
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../fs/fs_types.h"

int _fgetch(int fd)
{
        int ch;
        while (_read(fd, &ch, sizeof(ch)) == 0)
                halt();
        return ch;
}

char* _fgets(char *s, int n, int fd)
{
        int count = 0;
        char ch = 0;
        while ((n-- > 0) && (ch != '\n')) {
                ch = _fgetch(fd);
                
                // Handle backspace
                if (ch != '\b') {
                        *s++ = ch;
                        count++;
                } else
                        if (count > 0) {
                                *s-- = '\0';
                                count--;
                        }
                
        }
        return s;
                
}


int _fputch(char ch, int fd)
{
        return _write(fd, &ch, sizeof(ch));
}


int _fputs(char *s, int fd)
{
        int count = strlen(s);
        return _write(fd, s, count);
}

int STDIN = -1;
int STDOUT = -1;

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
                                character = (int) *arg++;
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

typedef struct shell_cmd_t {
        char name[16];
        shell_cmd_func cmd;
        char desc[100];
} shell_cmd_t;

struct shell_cmd_t shell_cmds[];

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
        if (argc < 2) {
                _printf("Usage: ls [path]\n");
                return;
        }
        
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));
        
        int fd = _open(argv[1], 0, 0);
        
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }
        
        _read(fd, directory, sizeof(directory));

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
        
        int fd = _open(argv[1], 0, 0);
        
        if (fd >= 0)
                _printf("Created regular file %s\n", argv[1]);
        else
                _printf("Failed to create regular file %s\n", argv[1]);

        _close(fd);
}

void shell_cmd_mkdir(int argc, char *argv[])
{
        if (argc < 2) {
                _printf("Usage: mkdir [path]\n", STDOUT);
                return;
        }

        char path[255];
        strncpy(path, argv[1], sizeof(path) - 2);
        strcat(path, "/");
        
        int fd = _open(path, 0, 0);
        
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
        
        int fd = _open(argv[1], 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }
        
        int ch;
        while (_read(fd, &ch, sizeof(ch)) != 0)
                _fputch(ch, STDOUT);
        
//        char buf[50];
//        memset(buf, 0, sizeof(buf));
//        _read(fd, buf, 5);
//        _fputs(buf, STDOUT);
//        
        _close(fd);
}

void shell_cmd_write(int argc, char *argv[])
{
        if (argc < 3) {
                _printf("Usage: write [file] [text]\n");
                return;
        }
        
        int fd = _open(argv[1], 0, 0);
        if (fd < 0) {
                _printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
                return;
        }
        
        //char buf[] = "HELLO WORLD.";
        //_write(fd, buf, 5);
        
        for (int i = 2; i < argc; i++) {
                _write(fd, argv[i], strlen(argv[i]));
                _write(fd, " ", 1);
        }
        _write(fd, "", 1);
        
        _printf("wrote %d bytes.\n", _seek(fd, 0, SEEK_CUR));
        
        _close(fd);
}


struct shell_cmd_t shell_cmds[] = {
                {"test",        shell_cmd_test,         "Test argument parsing"},
                {"cmdlist",     shell_cmd_cmdlist,      "List available commands"},
                {"echo",        shell_cmd_echo,         "Print text to STDOUT"},
                {"ls",          shell_cmd_ls,           "List directory"},
                {"touch",       shell_cmd_touch,        "Create regular file"},
                {"mkdir",       shell_cmd_mkdir,        "Create directory"},
                {"cat",         shell_cmd_cat,          "Print file contents"},
                {"write",       shell_cmd_write,        "Write text to file"},
                {"",            NULL,                   ""} // The Terminator
};

#define NUM_SHELL_COMMANDS (sizeof(shell_cmds) / sizeof(shell_cmd_t)) - 1 

void shell_handle_command(char *cmd) 
{
        // Kill the trailing line feed.
        cmd[strlen(cmd)-1] = '\0';
        
//        _fputs("- shell: cmd = ", STDOUT);
//        _fputs(cmd, STDOUT);
//        _fputs("\n", STDOUT);
        
        // Parsing setup
        char delim[] = " ";
        char *tok;
        char *copy = strdup(cmd); // uses kernel malloc
        char *work_copy = copy;

        // For the command
        int argc = 0;
        char *argv[16];
        
        while((tok = strsep(&work_copy, delim)) != NULL) {
                argv[argc++] = strdup(tok);
        }

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
        else {
                _fputs("- shell: ", STDOUT);
                _fputs(argv[0], STDOUT);
                _fputs(": command not found\n", STDOUT);
        }
        
        for (int i = 0; i < argc; i++)
                _free(argv[i]);
}

void shell_main()
{
        STDIN = _open("/dev/stdin", 0, 0);
        STDOUT = _open("/dev/stdout", 0, 0);
        
        _printf("\n\nWelcome to etiOS!\n");
        _printf("Try \"cmdlist\" for a list of commands.\n\n");
        
        // Prompt
        while (1) {
                _printf("$ ");
                char cmd[512];
                memset(cmd, 0, sizeof(cmd));
                _fgets(cmd, sizeof(cmd), STDIN);
                shell_handle_command(cmd);
        }
        
        _close(STDIN);
        _close(STDOUT);
        _exit(0);
}
