#include <stdio.h>
#include "logins.h"
#include "utils.h"


int main(void)
{
	int i;
	user_login_t *logins;

	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_meow[] = {TYPE_VOID};

	/* COMMAND(function name, alias, permission, argument types) */
	static command cmd_list[] = {
		COMMAND(add, "add", "debug", args_add),
		COMMAND(meow, "meow", "debug", args_meow),
	};






	return 0;
}
