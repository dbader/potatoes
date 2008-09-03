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
 * Interprets brainfuck source code.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../kernel/include/types.h"
#include "../kernel/include/const.h"
#include "../kernel/include/string.h"

#include "../kernel/include/debug.h"
#include "../kernel/include/assert.h"

#include "../kernel/io/io.h"
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/pm/syscalls_shared.h"
#include "brainfuck_interpreter.h"

int STDERR = 1;

int maxlength = 10000;
const int maxloopdepth = 50;
char *bf_start;
char *bf_ptr;
char *bf_buffer = NULL;
int bf_buf_offset = 0;
int bf_buf_position = -1;
int loop_marks[50][2];
bool jumpover = FALSE;
bool store_lock = FALSE;
int loopdepth=0;
int temploopdepth=0;
int store_lock_level=0;

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
                <[->+.<]\n\
                [-]++++++++++.";
        fd = _open("/bf_files/ascii.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        str="                   >\
                               + +\
                              +   +\
                             [ < + +\
                            +       +\
                           + +     + +\
                          >   -   ]   >\
                         + + + + + + + +\
                        [               >\
                       + +             + +\
                      <   -           ]   >\
                     > + + >         > > + >\
                    >       >       +       <\
                   < <     < <     < <     < <\
                  <   [   -   [   -   >   +   <\
                 ] > [ - < + > > > . < < ] > > >\
                [                               [\
               - >                             + +\
              +   +                           +   +\
             + + [ >                         + + + +\
            <       -                       ]       >\
           . <     < [                     - >     + <\
          ]   +   >   [                   -   >   +   +\
         + + + + + + + +                 < < + > ] > . [\
        -               ]               >               ]\
       ] +             < <             < [             - [\
      -   >           +   <           ]   +           >   [\
     - < + >         > > - [         - > + <         ] + + >\
    [       -       <       -       >       ]       <       <\
   < ]     < <     < <     ] +     + +     + +     + +     + +\
  +   .   +   +   +   +   [   -   ]   <   ]   +   +   +   +   +\
 * * * * * M a d e * B y : * N Y Y R I K K I * 2 0 0 2 * * * * *";
        fd = _open("/bf_files/sierpinski.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        str="+++++++++++ //number of fibbonacci numbers\
                >+>>>>++++++++++++++++++++++++++++++++++++++++++++\
                >++++++++++++++++++++++++++++++++<<<<<<[>[>>>>>>+>\
                +<<<<<<<-]>>>>>>>[<<<<<<<+>>>>>>>-]<[>++++++++++[-\
                <-[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]>[<<[>>>+<<<\
                -]>>[-]]<<]>>>[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]\
                >[<<+>>[-]]<<<<<<<]>>>>>[+++++++++++++++++++++++++\
                +++++++++++++++++++++++.[-]]++++++++++<[->-<]>++++\
                ++++++++++++++++++++++++++++++++++++++++++++.[-]<<\
                <<<<<<<<<<[>>>+>+<<<<-]>>>>[<<<<+>>>>-]<-[>>.>.<<<\
                [-]]<<[>>+>+<<<-]>>>[<<<+>>>-]<<[<+>-]>[<+>-]<<<-]\
                [-]++++++++++.";
        fd = _open("/bf_files/finitefib.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);
        
        str="++++[>+++++<-]>[<+++++>-]+<+[\
                >[>+>+<<-]++>>[<<+>>-]>>>[-]++>[-]+\
                >>>+[[-]++++++>>>]<<<[[<++++++++<++>>-]+<.<[>----<-]<]\
                <<[>>>>>[>>>[-]+++++++++<[>-<-]+++++++++>[-[<->-]+[<<<]]<[>+<-]>]<<-]<<-\
                ]";
        fd = _open("/bf_files/squares.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);
        
        str=" >>>>+>+++>+++>>>>>+++[\
        >,+>++++[>++++<-]>[<<[-[->]]>[<]>-]<<[\
        >+>+>>+>+[<<<<]<+>>[+<]<[>]>+[[>>>]>>+[<<<<]>-]+<+>>>-[\
        <<+[>]>>+<<<+<+<--------[\
        <<-<<+[>]>+<<-<<-[\
        <<<+<-[>>]<-<-<<<-<----[\
        <<<->>>>+<-[\
        <<<+[>]>+<<+<-<-[\
        <<+<-<+[>>]<+<<<<+<-[\
        <<-[>]>>-<<<-<-<-[\
        <<<+<-[>>]<+<<<+<+<-[\
        <<<<+[>]<-<<-[\
        <<+[>]>>-<<<<-<-[\
        >>>>>+<-<<<+<-[\
        >>+<<-[\
        <<-<-[>]>+<<-<-<-[\
        <<+<+[>]<+<+<-[\
        >>-<-<-[\
        <<-[>]<+<++++[<-------->-]++<[\
        <<+[>]>>-<-<<<<-[\
        <<-<<->>>>-[\
        <<<<+[>]>+<<<<-[\
        <<+<<-[>>]<+<<<<<-[\
        >>>>-<<<-<-\
        ]]]]]]]]]]]]]]]]]]]]]]>[>[[[<<<<]>+>>[>>>>>]<-]<]>>>+>>>>>>>+>]<\
        ]<[-]<<<<<<<++<+++<+++[\
        [>]>>>>>>++++++++[<<++++>++++++>-]<-<<[-[<+>>.<-]]<<<<[\
        -[-[>+<-]>]>>>>>[.[>]]<<[<+>-]>>>[<<++[<+>--]>>-]\
        <<[->+<[<++>-]]<<<[<+>-]<<<<\
        ]>>+>>>--[<+>---]<.>>[[-]<<]<\
        ]\
        [Enter a number using ()-./0123456789abcdef and space, and hit return.\
        DanielBCristofani(cristofd at hevanet dot com)\
        http://www.hevanet.com/cristofd/brainfuck/]";
        fd = _open("/bf_files/num2asciiart.b", O_CREAT, 0);
        _write(fd, str, strlen(str));
        _close(fd);

        bf_start = (char*)_malloc(maxlength);
        ASSERT(bf_start != NULL);
        int i;
        for(i=0; i<maxlength; i++)
                *(bf_start + i) = 0;
        bf_ptr = bf_start;
}

void reset_bf()
{       
        maxlength = 10000;
//        printf("_free(bf_start): 0x%x\n", bf_start);
//        _free(bf_start);
        if(bf_buffer != NULL) {
                printf("_free(bf_buffer): 0x%x\n", bf_buffer);
                _free(bf_buffer);
        }
        bf_buffer = NULL;
        bf_buf_offset = 0;
        bf_buf_position = -1;
        loopdepth = 0;
        temploopdepth=0;
        store_lock_level=0;
        store_lock = FALSE;
//        bf_start = (char*)_malloc(maxlength);
//        ASSERT(bf_start != NULL);
        int i;
        for(i=0; i<maxlength; i++)
                *(bf_start + i) = 0;
        bf_ptr = bf_start;
        jumpover = FALSE;
}

void interpret_bf(char ch)
{
        int start, end;
        
        //doing loop mode
        if (!store_lock && bf_buffer != NULL) {
                *(bf_buffer + bf_buf_offset) = ch;
                bf_buf_offset++;
                bf_buf_position++;
        }

        //jump over loop mode
        //matching closing bracket
        if (jumpover == TRUE && ch == ']' && loopdepth == temploopdepth) {
                jumpover = FALSE;
                loopdepth--;
                temploopdepth = 0;
                return;
        }
        //other symbol
        else if (jumpover == TRUE) {
                if (ch == '[') {
                        loopdepth++;
                }
                if (ch == ']') {
                        loopdepth--;
                }
                return;
        }

        //handle symbol mode        
        switch (ch) {
        case '+':
                ++*bf_ptr;
                break;
        case '-':
                --*bf_ptr;
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
                loopdepth++;
                if (*bf_ptr == 0) {
                        jumpover = TRUE;
                        temploopdepth = loopdepth;
                }
                else if (bf_buffer == NULL) {
                        bf_buffer = (char*)_malloc(maxlength);
                        ASSERT(bf_buffer != NULL);
                }
                else {
                        loop_marks[loopdepth][0] = bf_buf_position + 1;
                }
                break;
        case ']':
                loop_marks[loopdepth][1] = bf_buf_position;
                while (*bf_ptr != 0) {
                        store_lock = TRUE;
                        if(store_lock_level == 0) {
                                store_lock_level = loopdepth;
                        }
                        start = loop_marks[loopdepth][0];
                        bf_buf_position = start;
                        end = loop_marks[loopdepth][1];
                        while (start < end) {
                                interpret_bf(*(bf_buffer + start));
                                start++;
                                bf_buf_position = start;
                        }

                }
                if (loopdepth == store_lock_level) {
                        store_lock = FALSE;
                        store_lock_level = 0;
                }
                if (--loopdepth == 0 && bf_buffer != NULL) {
//                        printf("_free(bf_buffer): 0x%x", bf_buffer);
                        _free(bf_buffer);
                        bf_buffer = NULL;
                        bf_buf_offset = 0;
                        bf_buf_position = -1;
                        
                }
                break;
        }

        if(bf_ptr < bf_start){
                _write(STDERR, "LOWER BOUND ERROR\tREPAIRING\n", 
                                sizeof("LOWER BOUND ERROR\tREPAIRING\n"));
                maxlength *= 2;
                char* temp = _malloc(maxlength);
                memcpy(temp + (maxlength / 2), bf_start, maxlength / 2);
                //printf("_free(bf_start): 0x%x\n", bf_start);
                _free(bf_start);
                bf_start = temp;
                bf_ptr = bf_start + (maxlength / 2) - 1;
                //reset_bf();
        }
        else if (bf_ptr > bf_start + maxlength - 1){
                _write(STDERR, "UPPER BOUND ERROR\tREPAIRING\n", 
                                sizeof("UPPER BOUND ERROR\tREPAIRING\n"));
                maxlength *= 2;
                char* temp = _malloc(maxlength);
                memcpy(temp, bf_start, maxlength / 2);
                //printf("_free(bf_start): 0x%x\n", bf_start);
                _free(bf_start);
                bf_start = temp;
                bf_ptr = bf_start + (maxlength / 2);
                //reset_bf();
        }
}
