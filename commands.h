#ifndef USER_COMMANDS
#define USER_COMMANDS
#include "utils.h"


Value make_user(Value * list);
void test(void);

/**
 * @brief ncurses menu to select a user
 * @param allowed_utypes String array of what the user is allowed the select
 * @param n Size of allowed_utypes
 * @return String of selected username
 */
void user_select_menu(const char * const * allowed_utypes, int allowed_n, char * selected_username);
Value admin_view_cfg(Value * list);
#endif
