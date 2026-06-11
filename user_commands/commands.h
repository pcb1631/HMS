/**
 * @file commands.h
 * @brief User commands related to profile and reports
 *	
 *	When adding a command to the command array back in main.c, It needs to be either:
 * 	A function that returns Value with no args
 * 	A function that returns Value with a Value * arg (Value array)
 * 	
 * 	The length of the Value array, if used, should be tracked with the COMMAND macro back in the command array.
 * 	Arguments will be automatically supplied by command_select()
 * @author Josiah Hoo
 */
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
Value admin_view_cfg(void);
void print_cfg(Config cfg);
Value edit_profile_self_admin(void);
Value edit_profile_self_patient(void);
Value tedit_test(void);
Value doctor_diagnosis(void);
Value view_diagnosis_self_doc(void);
Value admin_delete_user(void); 
Value edit_patient_desc(void);
Value doctor_view_patients_profile(void);
#endif
