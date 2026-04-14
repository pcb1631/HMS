#include <stdio.h>
#include "logins.h"
#include <ncurses.h>



int main(void)
{
    initscr();
    start_color();
    printf("%d\n", has_colors());
	printf("%d\n",can_change_color());
	endwin();

	return 0;
}
