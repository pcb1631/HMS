#ifndef USER_COMMANDS
#define USER_COMMANDS
#include "../utils/utils.h"


Value make_user(Value * list);
void test(void);

/**
 * @brief ncurses menu to select a user
 * @param allowed_utypes String array of what the user is allowed to select
 * @param n Size of allowed_utypes
 * @param selected_username String to modify for selected username. Must be a char[n] or you'll get SIGSEGV (invalid permissions for mapped object)
 *
 */
void user_select_menu(const char * const * allowed_utypes, int allowed_n, char * selected_username);
Value admin_view_cfg(Value * list);
void print_cfg(Config cfg);
#endif
