/* $Id: int_handler.h 137 2008-06-15 22:04:35Z dbader $
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
 * Interprets brainfuck source code.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dbader $
 * @version $Rev: 137 $
 */

#include "../kernel/include/types.h"
#include "../kernel/include/const.h"
#include "../kernel/include/string.h"

#include "../kernel/include/debug.h"

#include "../kernel/io/io.h"
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/pm/syscalls_shared.h"
#include "brainfuck_interpreter.h"

int maxlength = 10000;
int recursion_depth = 50;
char *bf_start;
char *bf_ptr;
bool jumpover = FALSE;
char **loopbody;
int *loopptr;
sint8 loopdepth = -1;
int STDERR = 1;
bool *do_loop;
int bf_count_inner_loop = 0;
int bf_offset = 0;
int *bf_offsets;
int temp;
char* bf_str = 0;

void init_bf()
{
        char* str = "\
                ++++++++++\n\
                [\n\
                >+++++++>++++++++++>+++>+<<<<-\n\
                ]                       // Schleife zur Vorbereitung der Textausgabe\n\
                >++.                    // Ausgabe von 'H'\n\
                >+.                     // Ausgabe von 'e'\n\
                +++++++.                // 'l'\n\
                .                       // 'l'\n\
                +++.                    // 'o'\n\
                >++.            ;        // Leerzeichen\n\
                <<+++++++++++++++.      // 'W'\n\
                >.                      // 'o'\n\
                +++.                    // 'r'\n\
                ------.                 // 'l'\n\
                --------.               // 'd'\n\
                >+.                     // '!'\n\
                >.                      // Zeilenumbruch";
        _close(_open("/bf_files/", O_CREAT, 0));
        int fd = _open("/bf_files/helloworld.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        str=">++++++++++>+>+[\
                [+++++[>++++++++<-]>.<++++++[>--------<-]+<<<]>.>>[\
                [-]<[>+<-]>>[<<+>+>-]<[>+<-[>+<-[>+<-[>+<-[>+<-[>+<-\
                [>+<-[>+<-[>+<-[>[-]>+>+<<<-[>+<-]]]]]]]]]]]+>>>]<<<]";

        fd = _open("/bf_files/fib.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        str="\
                +++++++++>+++++++++>>>++++++[<+++>-]+++++++++>+++++++++>>>>>>+++++++++++++[<+++\
                +++>-]>+++++++++++[<++++++++++>-]<+>>++++++++[<++++>-]>+++++++++++[<++++++++++>\
                -]<->>+++++++++++[<++++++++++>-]<+>>>+++++[<++++>-]<-[<++++++>-]>++++++++++[<++\
                ++++++++>-]<+>>>+++++++[<+++++++>-]>>>++++++++[<++++>-]>++++++++[<++++>-]>+++++\
                ++++++[<+++++++++>-]<->>++++++++[<++++>-]>+++++++++++[<++++++++++>-]<+>>+++++++\
                +[<++++>-]>>+++++++[<++++>-]<+[<++++>-]>++++++++[<++++>-]>>+++++++[<++++>-]<+[<\
                ++++>-]>>++++++++++++[<+++++++++>-]++++++++++>>++++++++++[<++++++++++>-]<+>>+++\
                +++++++++[<+++++++++>-]>>++++++++++++[<+++++++++>-]>>++++++[<++++>-]<-[<+++++>-\
                ]>++++++++++++[<++++++++>-]<+>>>>++++[<++++>-]<+[<+++++++>-]>++++++++[<++++>-]>\
                ++++++++[<++++>-]>+++++++++++[<++++++++++>-]<+>>++++++++++[<++++++++++>-]<+>>>+\
                +++[<++++>-]<+[<++++++>-]>+++++++++++++[<++++++++>-]>++++++++[<++++>-]>>+++++++\
                [<++++>-]<+[<++++>-]>+++++++++++[<+++++++++>-]<->>++++++++[<++++>-]>++++++++++[\
                <++++++++++>-]<+>>+++++++++++[<++++++++++>-]>++++++++++[<++++++++++>-]<+>>+++++\
                ++++++[<++++++++++>-]<+>>>+++++[<++++>-]<-[<++++++>-]>++++++++[<++++>-]>+++++++\
                +++>>>>++++++++++++[<+++++++>-]++++++++++>>++++++++++++[<++++++++>-]<+>>+++++++\
                +++[<++++++++++>-]>++++++++++++[<+++++++++>-]<->>+++++++++++[<++++++++++>-]>+++\
                +++++++[<++++++++++>-]<+>>+++++++++++++[<+++++++++>-]>++++++++[<++++>-]>+++++++\
                ++++[<++++++++++>-]<+>>>>+++++[<++++>-]<-[<++++++>-]>+++++++++++[<++++++++++>-]\
                <+>>++++++++++++[<++++++++>-]<+>>+++++++++++[<++++++++++>-]>++++++++[<++++>-]>+\
                +++++++++[<++++++++++>-]<+>>>+++++++[<++++>-]<+[<++++>-]>>>+++++[<+++>-]<[<++++\
                +++>-]>>+++++[<+++>-]<[<+++++++>-]>++++++++[<++++>-]>>+++++++[<++++>-]<+[<++++>\
                -]>>++++++[<++++>-]<-[<+++++>-]>>>++++++[<++++>-]<-[<+++++>-]>++++++++[<++++>-]\
                >++++++++++++[<++++++++>-]<+>>++++++++++[<++++++++++>-]>>++++[<++++>-]<[<++++++\
                +>-]>+++++++++++[<++++++++++>-]<+>>++++++++[<++++>-]>>++++[<++++>-]<+[<+++++++>\
                -]>++++++++++[<++++++++++>-]>+++++++++++[<++++++++++>-]>+++++++++++[<++++++++++\
                >-]>++++++++[<++++>-]>++++++++++++[<++++++++>-]<+[<]<[<]<[<]<[<]<<<<[<]<[<]<[<]\
                <[<]<<<<[<]<<<<<<[>>[<<[>>>+>+<<<<-]>>>[<<<+>>>-]<[>+<-]>>-[>-<[-]]>+[<+>-]<<<]\
                >>[<<<<[-]>>>>>>>[>]>.>>>[.>>]>[>]>>[.>>]<[.<<]<[<]<<.>>>[.>>]>[>]>>[.>>]>.>>>[\
                .>>]>[>]>>[.>>]>>[.>>]<[.<<]<<<<[<]<[<]<[<]<[<]<<<<[<]>[.>]>>>>[.>>]>>[.>>]>>[.\
                >>]<[.<<]<[<]<<<<[<]<<-]<<<<[[-]>>>[<+>-]<<[>>+>+<<<-]>>>[<<<+>>>-]<[<<<+++++++\
                +[>++++++<-]>>>[-]]++++++++[<++++++>-]<<[.>.>]>[.>>]>>>[>]>>>>[.>>]>>[.>>]>>[.>\
                >]<[.<<]<[<]<<<<[<]<<<<<[.>.>]>[.>>]<<<[-]>[-]>>>>>[>]>>>>[.>>]>>[.>>]>>[.>>]>.\
                >>>[.>>]>>[.>>]>[>]>>[.>>]<[.<<]<<<<[<]<[<]<[<]<[<]<<<<[<]<<<<<<]<<[>+>+<<-]>>[\
                <<+>>-]<[>-<[-]]>+[<+>-]<[<++++++++++<->>-]<<[>>+>+>+<<<<-]>>[<<+>>-]<-[>+>+>>+\
                <<<<-]>[<+>-]>>>[<<[>>>+>+<<<<-]>>>[<<<+>>>-]<[>+<-]>>-[>-<[-]]>+[<+>-]<<<]>>[<\
                <<<[-]>>>>>>>[>]>.>>>[.>>]>[>]>>[.>>]<[.<<]<[<]<<<<[<]<<-]<<<<[[-]>>>[<+>-]<<[>\
                >+>+<<<-]>>>[<<<+>>>-]<[<<<++++++++[>++++++<-]>>>[-]]++++++++[<++++++>-]<<[.>.>\
                ]>[.>>]<<<[-]>[-]>>>>>[>]>>>>[.>>]>>[.>>]>>[.>>]<[.<<]<[<]<<<<[<]<<<<<<]+++++++\
                +++.[-]<<<[>>+>+>+<<<<-]>>[<<+>>-]<[>+>+>>+<<<<-]>[<+>-]>]\n";
        fd = _open("/bf_files/beer.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        str="\
                ++\
                [>++<-]\n\
                >[<++>-]\n\
                <[>++<-]\n\
                >[<++>-]\n\
                <[>++<-]\n\
                >[<++>-]\n\
                <[->+.<]\n";
        fd = _open("/bf_files/ascii.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        bf_start = (char*)_malloc(maxlength);
        loopbody = (char**)_malloc(recursion_depth*sizeof(char*));
        loopptr = (int*)_malloc(recursion_depth*sizeof(int));
        bf_offsets = (int*)_malloc(recursion_depth*sizeof(int));
        do_loop = (bool*)_malloc(recursion_depth*sizeof(bool));
        int i;
        for(i=0; i<maxlength; i++)
                *(bf_start + i) = 0;
        bf_ptr = bf_start + (maxlength / 2);
}

void reset_bf()
{       
        maxlength = 10000;
        _free(bf_start);
        _free(loopbody);
        _free(loopptr);
        bf_start = (char*)_malloc(maxlength);
        loopbody = (char**)_malloc(recursion_depth*sizeof(char*));
        loopptr = (int*)_malloc(recursion_depth*sizeof(int));
        int i;
        for(i=0; i<maxlength; i++)
                *(bf_start + i) = 0;
        bf_ptr = bf_start;
        do_loop = FALSE;
        jumpover = FALSE;
        loopdepth = -1;
}

void interpret_bf(char ch)
{
        int i;
        //cputchar(ch,BLUE,YELLOW);

        for(i=0; i<=loopdepth; i++)
                if(     !do_loop[loopdepth] &&
                                !do_loop[i] &&
                                !(!jumpover && i==loopdepth && ch==']')
                )
                        *(loopbody[i] + loopptr[i]++) = ch;

        if(jumpover && ch == '['){
                bf_count_inner_loop++;
                return;
        }
        else if(jumpover && ch == ']' && bf_count_inner_loop != 0){
                bf_count_inner_loop--;
                return;
        }
        else if(jumpover && ch != ']'){
                return;
        }
        else if(jumpover){
                jumpover = FALSE; 
                return;
        }

        //if(!do_loop[loopdepth] && ch!=']')
        //        *(loopbody[loopdepth] + loopptr[loopdepth]++) = ch;
        //        if(loopdepth>=0 && !do_loop[loopdepth]){
        //                *(loopbody[loopdepth] + loopptr[loopdepth]++) = ch;
        //        }

        switch(ch){
        case '+':
                ++*bf_ptr;
                break;
        case '-':
                --*bf_ptr;
                //printf("%{-----}%d%{-----}\n",RED,*bf_ptr,RED);
                break;
        case '<':
                --bf_ptr;
                break;
        case '>':
                ++bf_ptr;
                break;
        case '.':
                _write(1, bf_ptr, 1);
                break;
        case ',': 
                while (_read(2, bf_ptr, 1) == 0)
                        halt();
                break;
        case '[':
                if(*bf_ptr == 0) jumpover = TRUE;
                else{
                        if(loopdepth >= 0 && do_loop[loopdepth-bf_offset]){
                                bf_offset++;
                        }
                        loopdepth++;
                        loopptr[loopdepth]=0;
                        do_loop[loopdepth]=FALSE;
                        loopbody[loopdepth] = (char*)_malloc(maxlength);
                        for(i=0; i<maxlength; i++)
                                *(loopbody[loopdepth] + i) = 0;
                }
                break;
        case ']':
                bf_offsets[loopdepth] = bf_offset;
                bf_offset = 0;
                while(*bf_ptr != 0){
                        do_loop[loopdepth] = TRUE;
                        //printf("loopdepth: %d\n",loopdepth);
                        for(i=0; i<loopptr[loopdepth-bf_offset]; i++){
                                interpret_bf(*(loopbody[loopdepth-bf_offset]+i));
                        }
                        //_write(BRAINFUCK,loopbody[tbemploopdepth],strlen(loopbody[temploopdepth]));
                }

                bf_offset = bf_offsets[loopdepth];

                //                dprintf("loopdepth = %d\tbf_offset = %d\npos = %d\tvalue = %d:\n",loopdepth, bf_offset, bf_ptr-bf_start, *bf_ptr);
                //                for(i=0; i<=loopdepth; i++)
                //                        printf("%s\n\n",loopbody[i]);
                //                dprint_separator();

                _free(loopbody[loopdepth]);
                do_loop[loopdepth] = FALSE;
                loopdepth--;
                if(bf_offset>0) bf_offset--;
        }

        if(bf_ptr < bf_start){
                _write(STDERR, "LOWER BOUND ERROR\n", sizeof("LOWER BOUND ERROR\n"));
                maxlength *= 2;
                char* temp = _malloc(maxlength);
                memcpy(temp + (maxlength / 2), bf_start, maxlength / 2);
                _free(bf_start);
                bf_start = temp;
                bf_ptr = bf_start + (maxlength / 2) - 1;
                //reset_bf();
        }
        else if (bf_ptr > bf_start + maxlength - 1){
                _write(STDERR, "UPPER BOUND ERROR\n", sizeof("UPPER BOUND ERROR\n"));
                maxlength *= 2;
                char* temp = _malloc(maxlength);
                memcpy(temp, bf_start, maxlength / 2);
                _free(bf_start);
                bf_start = temp;
                bf_ptr = bf_start + (maxlength / 2);
                //reset_bf();
        }
}
