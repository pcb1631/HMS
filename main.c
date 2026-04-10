#include <stdio.h>
#include "logins.h"
#include "tui.h"
#include "utils.h"

#ifndef _WIN32
	#include <ncurses.h>
#else
	#include <ncurses/curses.h>
#endif

int main(void)
{
	int i;
	char * pw_file = "./users/creds.txt";
	char * options[3] = {
		"Login\0",
		"Register\0",
		"Exit\0"
	};
	char selection[20] = "";

	/*Pass to logins.h functions as a pointer to hash pointer, as per uthash doc

	Defined in main because global variables aren't allowed */
	user_login_t* logins= NULL;

	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_meow[] = {TYPE_VOID};

	/* COMMAND(function name, alias, permission, argument types) */
	static command cmd_list[] = {
		COMMAND(add, "add", "debug", args_add),
		COMMAND(meow, "meow", "debug", args_meow),
	};





	switch (simple_select(COLOR_MAGENTA, COLOR_BLACK, 3, options, selection))
	{
		case 0: printf("fee"); break;
		case 1: printf("foo"); break;
		case 2: return 0;
	}


	return 0;
}
