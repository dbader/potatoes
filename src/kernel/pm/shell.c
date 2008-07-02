#include "syscalls_shared.h"
#include "syscalls_cli.h"
#include "../io/io.h"
#include "../include/string.h"
#include "../include/const.h"
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../fs/fs_types.h"

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
        
        if (ch == '\n')
                *s++ = ch;
        
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
        
        if (_read(fd, directory, sizeof(directory)) != sizeof(directory)) {
                _close(fd);
                _printf("%s: Error reading directory\n", argv[0]);
        }

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
        
        // Append a trailing slash to let open() know we want to
        // create a directory.
        if (path[strlen(path)] != '/')
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
        while (_read(fd, &ch, sizeof(ch)) != 0)
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
                
        for (int i = 2; i < argc; i++) {
                _write(fd, argv[i], strlen(argv[i]));
                _write(fd, " ", 1);
        }
        
        _write(fd, "", 1); // append \0
        _printf("wrote %d bytes.\n", _seek(fd, 0, SEEK_CUR));       
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
        for (int i = 0; i < 25 * 80; i++)
                _fputs("\b", STDOUT);
}

extern void fs_shutdown();
extern void fs_init();
extern bool create_fs();

void shell_cmd_sync(int argc, char *argv[])
{
        fs_shutdown();
        create_fs();
        fs_shutdown();
        fs_init();
}

extern void mem_dump();
void shell_cmd_memdump(int argc, char *argv[])
{
        mem_dump();
}

// TODO: nice to have:
/*
 * tab completion
 * repeat last command
 * pwd
 * cp
 * rm
 * mv
 * ps
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
                {"memdump",     shell_cmd_memdump,     "Dump allocated blocks"},
                {"",            NULL,                   ""} // The Terminator
};

#define NUM_SHELL_COMMANDS (sizeof(shell_cmds) / sizeof(shell_cmd_t)) - 1 

void shell_handle_command(char *cmd) 
{       
        if (strlen(cmd) == 1)
                return;
        
        // Kill the trailing line feed.
        // FIXME: Dont know why we get that twice. ???
        cmd[strlen(cmd)-1] = '\0';
        cmd[strlen(cmd)-1] = '\0';
        
        // Parsing setup
        char delim[] = " ";
        char *tok;
        char *copy = strdup(cmd); // uses kernel malloc
        char *work_copy = copy;

        // For the command
        int argc = 0;
        char *argv[16];
        
        while((tok = strsep(&work_copy, delim)) != NULL) {
                if (argc >= (sizeof(argv) / sizeof(char*))) {
                        _printf("- shell: argument overflow. Last argument: %s\n", argv[argc-1]);
                        break;
                }
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
        else
                _printf("- shell: %s: command not found\n", argv[0]);
        
        for (int i = 0; i < argc; i++)
                _free(argv[i]);
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
