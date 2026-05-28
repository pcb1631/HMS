#include <stdio.h>
#include <stdlib.h>
#include "utils/string_utils.h"
#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#define OPTIONS 64

int main(void)
{
	char ** strings = NULL;
	int n;
	int i;
	strings = file_to_strings("/", &n);
	if(strings == NULL)
	{
		return -1;
	}
	for(i=0; i<n; i++)
	{
		printf("%s\n", strings[i]);
	}
	for(i=0; i<n; i++)
	{
		free(strings[i]);
	}
	free(strings);
	return 0;
}
