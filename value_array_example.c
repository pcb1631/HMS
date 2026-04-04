#include "utils.h"
#include <stdio.h>

int main(void)
{
	Value list[2];
	Value list2[3];
	const char * s = "Three";

	list[0] = make_int(10);
	list[1] = make_int(20);

	printf("%d\n",add(list));



	list2[0] = make_int(1);
	list2[1] = make_float(2.2f);
	list2[2] = make_string("Threee");


	/* list2 automatically decays to &list2[0] (pointer to first element) */
	print_list(list2, sizeof(list2) / sizeof(list2[0]));

	return 0;
}
