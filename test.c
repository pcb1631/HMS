#include <stdio.h>
#include <stdlib.h>
#include "utils/string_utils.h"

#define OPTIONS 64

int main(void)
{
	int n;
	LineNode * Head;
	LineNode * Node;
	Head = file_to_LL_lines("./users/admin/profile.txt", &n);
	if(Head == NULL)
		exit(1);
	Node = Head;

	while(Node != NULL)
	{
		printf("%s\n", Node->str);
		Node = Node->next;
	}

	free_LineLL(Head);

	return 0;
}
