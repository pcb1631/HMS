#include "utils.h"
#include <stdio.h>

int main(void)
{
	Value list[2];
	Value list2[3];
	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_meow[] = {TYPE_VOID};
	static command cmd_list[] = {
		COMMAND(add, "add", "debug", args_add),
		COMMAND(meow, "meow", "debug", args_meow),
	};
	Value result;
	char * current_perm = "debug";
	Value args[2];
		args[0] = make_int(2);
		args[1] = make_int(3);

	list[0] = make_int(10);
	list[1] = make_int(20);

	printf("%d\n",add(list).data.as_int);

	list2[0] = make_int(1);
	list2[1] = make_float(2.2f);
	list2[2] = make_string("Threee");




	/* list2 automatically decays to &list2[0] (pointer to first element) */
	print_list(list2, sizeof(list2) / sizeof(list2[0]));






	result = call(cmd_list[0], args, sizeof(args)/sizeof(Value), current_perm);

	printf("%d\n", result.data.as_int);





	return 0;
}
