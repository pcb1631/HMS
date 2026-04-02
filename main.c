#include <stdio.h>
#include "utils.h"

typedef void (*generic_fn)(void);

struct shorthand_fn_pair /* implement hashmap later */
{
	char command[15];
	generic_fn fn_ptr;
};




int main(void)
{
	char current_UUID[37];
	char current_username[50];
	char current_utype[10];

	char str[] = "token1 token2 bla bla wa";
	cmd_parse((char *)&str);


	return 0;
}
