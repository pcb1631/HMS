#include <ncurses.h>
#include <string.h>

int simple_select(int color1, int color2, int n_of_options, char ** options, char * selection_s)
{
	int ch = 0;
	int i;
	int selection;

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, 1);

	while(ch != '\n')
	{
		for(i=0; i<n_of_options; i++)
		{
			if(i == selection)
				attron(A_REVERSE);

			mvprintw(i, 0, options[i]);

			if(i == selection)
				attroff(A_REVERSE);
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
