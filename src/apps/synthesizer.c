/* $Id: io_synthesizer.c 239 2008-10-26 23:19:15Z dtraytel $
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

#include "shell_utils.h"

#include "../kernel/include/string.h"
#include "../kernel/include/stdlib.h"

/**
 * Stores a frequency and a duration.
 */
typedef struct {
        int frequency;
        int duration;
} Tone;

/**
 * Parse a sound string.
 *
 * This will parse a sound string like "C1 1 D1 2 CSH2 3", allocate a new
 * buffer and return it, along with a sentinel that consists of a tone with its
 * duration set to -1.
 *
 * As an extension, it also supports pauses, represented by a number without a
 * prefixed note. For example, "C1 1 2 D1 3" represents the note C1 for 1 tick,
 * 2 ticks worth of pausing, and D1 for 3 ticks. Pauses are represented by a
 * frequency of 0.
 *
 * This function will break if it encounters an error, and return all the data
 * that has been parsed so far.
 *
 * @param str the null-terminated string to be parsed
 * @returns the resulting tones, to be freed with free()
 */
static Tone *parse_sound(char *str) {
        int length = 0; // Number of tones that have been parsed
        int max_length = 16; // Number of tones that we can store
        Tone *ret = (Tone *) mallocn(max_length * sizeof(Tone), "parse_sound() buffer");

        while(1) {
                // Eat leading white space
                while(*str != '\0' && isspace(*str))
                        ++str;

                // Find the longest string in note_names that matches the one
                // at our current position
                int best_match_length = 0;
                int best_note;
                for(int note = 0; note < (sizeof(note_names)/ sizeof(note_names[0])); ++note) {
                        int i;
                        for(i = 0; str[i] != '\0' && note_names[note][i] == str[i]; ++i) ;
                        if(i > best_match_length) {
                                best_match_length = i;
                                best_note = note;
                        }
                }

                str += best_match_length; // Advance so we can continue
                                         // parsing.

                if(*str < '0' || *str > '9')
                        break; // What follows must always be a number, either
                              // the octave or the duration of the pause.

                Tone tone;
                if(best_match_length <= 0) {
                        tone.frequency = 0; // an empty note means a pause.
                } else {
                        int octave = strtol(str, &str, 10);

                        if(octave < 0 || octave > 7)
                                break;

                        tone.frequency = notes[best_note][octave];
                }

                tone.duration = strtol(str, &str, 0);
                if(tone.duration < 0) {
                        break;
                }

                // Exponentially expand the size of our array if necessary.
                // Reserve one entry for the next note and one for the
                // sentinel.
                if(length + 2 >= max_length) {
                        max_length *= 2;
                        Tone *new_ret = (Tone *) mallocn(max_length * 2 * sizeof(Tone), "parse_sound() buffer");
                        memcpy(new_ret, ret, length);
                        free(ret);
                        ret = new_ret;
                }

                // Append the note we just parsed.
                ret[length].frequency = tone.frequency;
                ret[length].duration = tone.duration;
                ++length;
        }

        // add sentinel (this is safe to do w/o range checking since we already
        // reserved one entry)
        ret[length].frequency = 0;
        ret[length].duration = -1;

        return ret;
}

static void handle_note(int keyboard, int key, int note, int octave){
        int note_length = 0;
        start_beep(notes[note][octave]);
        while(keydown(key,keyboard)) {
                note_length++;
                halt();
        }

        if(note >= 0 && note < 12)
                _printf("%s%d %d ", note_names[note], octave, note_length);

        end_beep();
}

static void play_tone(Tone *tone) {
        if(tone->frequency > 0)
            start_beep(tone->frequency);

        int duration;
        for(duration = tone->duration; duration > 0; --duration)
                halt();

        if(tone->frequency > 0)
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
                if(keydown(KEY_K,keyboard) && octave < 7) {
                        handle_note(keyboard, KEY_K, NOTE_C, octave+1);
                }
        }
        _close(keyboard);
        _exit(0);
}

void shell_cmd_synth(int argc, char *argv[])
{
        if(argc > 1) {
                int fd = _open(shell_makepath(argv[1]), 0, 0);
                if(fd <= 0) {
                        _printf("synth: can't open %s for reading\n", argv[1]);
                        return;
                }

                int size = _seek(fd, 0, SEEK_END);
                if(size < 0) {
                        _printf("synth: %s has a negative size. Trying to open a special file?\n", argv[1]);
                        return;
                }
                _seek(fd, 0, SEEK_SET);

                char *str = malloc(size + 1);
                _read(fd, str, size);
                str[size] = '\0';

                Tone *tones = parse_sound(str);

                Tone *tone;
                for(tone = tones; tone->duration >= 0; ++tone)
                        play_tone(tone);

                free(tones);
                free(str);

                return;
        } else {
            pm_create_thread("SYNTH", synth, 4096);
        }
}
