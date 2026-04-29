#include "utils.h"
#include <stdio.h>
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
		mvprintw(0, 0, "%s", pretext);
		attroff(COLOR_PAIR(1));

		for(i=0; i<n_of_options; i++)
		{
			if(i == selection)
				attron(A_REVERSE);
			attron(COLOR_PAIR(1));

			mvprintw(i+1, 0, "%s", options[i]);

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
				selection = (selection - 1 % n_of_options + n_of_options) % n_of_options; break;
			case KEY_DOWN:
			case KEY_RIGHT:
				selection = (selection + 1 % n_of_options + n_of_options) % n_of_options; break;
			case '\n':
				endwin(); break;
		}
	}

	strcpy(selection_s, options[selection]);

	return selection;
}

void command_select(command_node *head, char *utype, char *username)
{
    int selection = 0;
    int i;
    int ch = 0;
   	WINDOW *left_win, *right_win;
    int y_max, x_max;

    command cmd_list[30];
    int cmd_list_n;
    command_node* Node;

    Value placeholder[1];
    placeholder[0] = make_int(0);

    i = 0;
    Node = head;
    while(Node != NULL)
    {
        cmd_list[i] = Node->f;
        i++;
        Node = Node->next;
    }
    cmd_list_n = i;

    while(1)
    {
        initscr();
        cbreak();
        noecho();
        curs_set(0);
        keypad(stdscr, 1);
        start_color();

       	init_color(10, 106*(1000/255), 90*(1000/255), 205*(1000/255));
        init_pair(1, COLOR_WHITE, 10);
        getmaxyx(stdscr, y_max, x_max);

        left_win = newwin(y_max, x_max/2, 0, 0);
        box(left_win, 0 ,0);
        keypad(left_win, 1);

        right_win = newwin(y_max, (x_max/2), 0, (x_max/2));

        /*
        box(right_win, 0, 0);
         */

        /*
        mvwprintw(left_win, 0,0, "%c %d", ch, selection);
         */


        mvwprintw(left_win ,0, 0, "%s %s", username, utype);

        for(i=0; i<cmd_list_n; i++)
        {

            if(i == selection)
				wattron(left_win, A_REVERSE);

            mvwprintw(left_win, i+1, 1, "%s", cmd_list[i].alias);

            if(i == selection)
				wattroff(left_win, A_REVERSE);
        }


        wbkgd(left_win, COLOR_PAIR(1));
        wrefresh(left_win);

        mvwprintw(right_win, 1, 1, "Permission: %s", cmd_list[selection].permission);

        /*
         *
        display types and desc
         */
        {
            char * taip;
            for(i = 0; i<cmd_list[selection].n_of_args; i++)
            {
                switch(cmd_list[selection].arg_type[i])
                {
                    case TYPE_INT: taip = "integer"; break;
                    case TYPE_FLOAT: taip = "decimal"; break;
                    case TYPE_STRING: taip = "string"; break;
                    case TYPE_BOOL: taip = "0 or 1"; break;
                    case TYPE_VOID: taip = "Nothing"; break;
                }
                mvwprintw(right_win, i+3, 1, "Argument %d: %s", i, taip);
            }

            mvwprintw(right_win, i+4, 1, "Description:");
            mvwprintw(right_win, i+5, 1, "%s" ,cmd_list[selection].desc);
        }

        wrefresh(right_win);

        ch = wgetch(left_win);

        switch(ch)
        {
            case KEY_LEFT:
            case KEY_UP:
                selection = (selection - 1 % cmd_list_n + cmd_list_n) % cmd_list_n;
                break;
            case KEY_RIGHT:
            case KEY_DOWN:
                selection = (selection + 1 % cmd_list_n + cmd_list_n) % cmd_list_n;
                break;

            case KEY_RESIZE:
                break;

            case '\n':
                delwin(right_win);
                delwin(left_win);
                endwin();

                if(strcmp(utype, cmd_list[selection].permission) != 0)
                {
                    printf("Permission denied (current perm: %s, command perm: %s", utype, cmd_list[selection].permission);
                    getchar();
                    break;
                }


                if (cmd_list[selection].n_of_args > 0)
                {
                    Value * args;
                    int i;
                    int n = cmd_list[selection].n_of_args;
                    args = malloc(sizeof(Value) * n);


                    for(i=0; i<n; i++)
                    {
                        args[i].type = cmd_list[selection].arg_type[i];
                        printf("Argument %d:\n", i);
                        switch (args[i].type)
                        {
                            case TYPE_BOOL:
                                printf("Using int as bool\n");
                            case TYPE_INT:
                                printf("Enter an integer\n");
                                scanf("\n%d", &args[i].data.as_int); break;

                            case TYPE_STRING:
                                printf("Enter a string\n");
                                scanf("\n%s", args[i].data.as_string); break;

                            case TYPE_FLOAT:
                                printf("Enter a decimal");
                                scanf("\n%f", &args[i].data.as_float); break;

                            case TYPE_VOID:
                                printf("Void\n"); break;
                            default:
                                break;;
                        }
                        printf("\n");
                    }

                    call(cmd_list[selection], args, cmd_list[selection].n_of_args, utype);
                    free(args);
                    getchar();

                }else
                    call(cmd_list[selection], placeholder, 1, utype);
                getchar();
                break;

            default:
                break;
        }
    }
}
