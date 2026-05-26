#ifndef TUI_H
#define TUI_H

/**
 * @file tui.h
 * @brief Text User Interface functions.
 */

#include "utils.h"
#include "../user_commands/bookings.h"
#include <time.h>
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
int simple_select(int color_foreground, int color_background, int n_of_options, const char * const * options, char * selection_s, const char * pretext);

/**
 * @brief The command select menu that is shown after login.
 *
 * To only show the commands that the user is allowed to use, this function takes in a singly null terminated LL of Command variables.
 *
 *
 */
void command_select(command_node* head, char * utype, char * username);

/**
 * @brief TUI for picking a specific time.
 * @param pretext Text to show on top
 * @param pre_time Default time before user interacts
 * @return UNIX timestamp (seconds) of what user chose
 */
time_t time_picker(char * pretext, int color_fg, int color_bg, time_t pre_time);

/**
 * @brief TUI Viewer/selection menu for appointment/booking slots
 * @param slots Slot array
 * @param slots_n Size of Slot array
 * @param doctor username of Doctor
 * @param is_patient If true, details of slots that are occupied by other patients are not shown for privacy.
 * @return Index of selection of Slot array. return -1 if no slots.
 */
int booking_slot_view(Slot * slots, int slots_n, const char * doctor, int is_patient);
int config_editor(Config config, const char * path, const char * const * disallow, int disallow_n);
void ncurses_init(void);
#endif
