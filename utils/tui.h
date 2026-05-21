#ifndef TUI_H
#define TUI_H

/**
 * @file tui.h
 * @brief Text User Interface functions.
 */

/**
 * @brief Simple select TUI made with ncurses
 * @param color_foreground Foreground color pair
 * @param color_background Background color pair
 * @param n_of_options Number of options
 * @param options Array of option strings
 * @param selection_s String to modify for selection. Must be char[n]
 * @param pretext Text to display before options
 * @return Index of the selected option
 */
#include "utils.h"
#include "../user_commands/bookings.h"
#include <time.h>
int simple_select(int color_foreground, int color_background, int n_of_options, const char * const * options, char * selection_s, const char * pretext);

void command_select(command_node* head, char * utype, char * username);

time_t time_picker(char * pretext, int color_fg, int color_bg, time_t pre_time);
int booking_slot_view(Slot * slots, int slots_n, const char * doctor, bool patient);
#endif
