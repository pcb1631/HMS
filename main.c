#include <stdio.h>
#include "utils.h"








int main(void)
{
	int i;

	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_meow[] = {TYPE_VOID};

	/* COMMAND(function name, alias, permission, argument types) */
	static command cmd_list[] = {
		COMMAND(add, "add", "debug", args_add),
		COMMAND(meow, "meow", "debug", args_meow),
	};

	int cmd_list_size = sizeof(cmd_list) / sizeof(command);

	for(i = 0; i<cmd_list_size; i++)
	{
		printf("Command %d: %s  Permission: %s \n", i, cmd_list[i].alias, cmd_list[i].permission);
	}


	char current_UUID[37];
	char current_username[50];
	char current_utype[10];



	return 0;
}
