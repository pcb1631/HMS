#include <stdio.h>
#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#define OPTIONS 64

int main(void)
{
	char string[] = "abcdefg\0\b\0";
	int i = 3;
	printf("%s\n", string);
	while(string[i] != '\0')
	{
		string[i] = string[i+1];
		i++;
	}
	printf("%s\n", string);
	return 0;
}
