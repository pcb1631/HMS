#include <stdio.h>

#ifndef _WIN32
#include <ncurses.h>
#else
#include <ncurses/curses.h>
#endif


int main(void)
{
    initscr();
    start_color();
    printf("%d\n", has_colors());
	printf("%d\n",can_change_color());
	endwin();

	return 0;
}
