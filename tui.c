#ifndef _WIN32
	#include <ncurses.h>
	#include <stdlib.h>
#else
	#include <ncurses/curses.h>
#endif
#include <string.h>
#include "tui.h"

int simple_select(int color_foreground, int color_background, int n_of_options, char ** options, char * selection_s, const char * pretext)
{
	int ch = 0;
	int i;
	int selection=0;

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, 1);

	start_color();
	init_pair(1, color_foreground, color_background);

	while(ch != '\n')
	{
		attron(COLOR_PAIR(1));
		mvprintw(0, 0, pretext);
		attroff(COLOR_PAIR(1));

		for(i=0; i<n_of_options; i++)
		{
			if(i == selection)
				attron(A_REVERSE);
			attron(COLOR_PAIR(1));

			mvprintw(i+1, 0, options[i]);

			if(i == selection)
				attroff(A_REVERSE);
			attroff(COLOR_PAIR(1));
		}

		refresh();
		ch = getch();

		switch(ch)
		{
			case KEY_UP:
			case KEY_LEFT:
				selection = (selection - 1 % 3 + 3) % 3; break;
			case KEY_DOWN:
			case KEY_RIGHT:
				selection = (selection + 1 % 3 + 3) % 3; break;

			case '\n':
				endwin(); break;
		}
	}

	strcpy(selection_s, options[selection]);

	return selection;
}
