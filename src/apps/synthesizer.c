/* $Id: io_synthesizer.c 239 2008-10-26 23:19:15Z dtraytel $
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
 * Synthesizer tool
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 239 $
 */

#include "../kernel/io/io.h"
#include "../kernel/pm/pm_main.h"
#include "../kernel/pm/syscalls_cli.h"
#include "games.h"

static void handle_note(int keyboard, int key, int note, int octave){
        int note_length = 0;
        start_beep(notes[note][octave]);
        while(keydown(key,keyboard)) {
                note_length++;
                halt();
        }
        if(note == NOTE_C) {
                _printf("C%d %d ", octave, note_length);
        } else if(note == NOTE_CSH) {
                _printf("CSH%d %d ", octave, note_length);
        } else if(note == NOTE_D) {
                _printf("D%d %d ", octave, note_length);
        } else if(note == NOTE_DSH) {
                _printf("DSH%d %d ", octave, note_length);
        } else if(note == NOTE_E) {
                _printf("E%d %d ", octave, note_length);
        } else if(note == NOTE_F) {
                _printf("F%d %d ", octave, note_length);
        } else if(note == NOTE_FSH) {
                _printf("FSH%d %d ", octave, note_length);
        } else if(note == NOTE_G) {
                _printf("G%d %d ", octave, note_length);
        } else if(note == NOTE_GSH) {
                _printf("GSH%d %d ", octave, note_length);
        } else if(note == NOTE_A) {
                _printf("A%d %d ", octave, note_length);
        } else if(note == NOTE_ASH) {
                _printf("ASH%d %d ", octave, note_length);
        } else if(note == NOTE_B) {
                _printf("B%d %d ", octave, note_length);
        } else if(note == NOTE_C) {
                _printf("C%d %d ", octave, note_length);
        }
        end_beep();
}

void synth() {
        int keyboard = _open("/dev/keyboard", 0, 0);
        int octave = 4;
        bool nextoct = 0;
        while (!keydown(ESCAPE, keyboard)) {
                if(keydown(KEY_MINUS,keyboard) && octave>0) {
                        octave--;
                        while(keydown(KEY_MINUS,keyboard));
                }
                if(keydown(KEY_PLUS,keyboard) && octave<7) {
                        octave++;
                        while(keydown(KEY_PLUS,keyboard));
                }
                if(keydown(KEY_A,keyboard)) {
                        handle_note(keyboard, KEY_A, NOTE_C, octave);
                }
                if(keydown(KEY_W,keyboard)) {
                        handle_note(keyboard, KEY_W, NOTE_CSH, octave);
                }
                if(keydown(KEY_S,keyboard)) {
                        handle_note(keyboard, KEY_S, NOTE_D, octave);
                }
                if(keydown(KEY_E,keyboard)) {
                        handle_note(keyboard, KEY_E, NOTE_DSH, octave);
                }
                if(keydown(KEY_D,keyboard)) {
                        handle_note(keyboard, KEY_D, NOTE_E, octave);
                }
                if(keydown(KEY_F,keyboard)) {
                        handle_note(keyboard, KEY_F, NOTE_F, octave);
                }
                if(keydown(KEY_T,keyboard)) {
                        handle_note(keyboard, KEY_T, NOTE_FSH, octave);
                }
                if(keydown(KEY_G,keyboard)) {
                        handle_note(keyboard, KEY_G, NOTE_G, octave);
                }
                if(keydown(KEY_Z,keyboard)) {
                        handle_note(keyboard, KEY_Z, NOTE_GSH, octave);
                }
                if(keydown(KEY_H,keyboard)) {
                        handle_note(keyboard, KEY_H, NOTE_A, octave);
                }
                if(keydown(KEY_U,keyboard)) {
                        handle_note(keyboard, KEY_U, NOTE_ASH, octave);
                }
                if(keydown(KEY_J,keyboard)) {
                        handle_note(keyboard, KEY_J, NOTE_B, octave);
                }
                if(keydown(KEY_K,keyboard)) {
                        handle_note(keyboard, KEY_K, NOTE_C, octave+1);
                }
        }
        _close(keyboard);
        _exit(0);
}

void shell_cmd_synth(int argc, char *argv[])
{
        pm_create_thread("SYNTH", synth, 4096);
}