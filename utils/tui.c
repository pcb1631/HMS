#include "tui.h"
#include "globals.h"
#include "string_utils.h"
#include "utils.h"
#include <stdio.h>
#ifndef _WIN32
	#include <ncurses.h>
	#include <stdlib.h>
#else
	#include <ncurses/curses.h>
#endif
#include <string.h>
#include <time.h>

#define KEY_ETX 0x03 /* CTRL+C */
#define KEY_EOT 0x04 /* CTRL+D */
#define KEY_CTRL_W 0x17

void ncurses_init(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, 1);
    start_color();
}

int simple_select(int color_foreground, int color_background, int n_of_options, const char* const* options, char * selection_s, const char * pretext)
{
	int ch = 0;
	int i;
	int selection=0;
	int y_max;

	ncurses_init();
	init_pair(1, color_foreground, color_background);

	while(ch != '\n')
	{
	    clear();
	    y_max = getmaxy(stdscr);

		attron(COLOR_PAIR(1));
		mvprintw(0, 0, "%s", pretext);
		attroff(COLOR_PAIR(1));

		for(i=0; i<n_of_options; i++)
		{
		    if (i-selection + (y_max/2) < 1) /* if its gonna cover the pretext, just skip */
				continue;

			if(i == selection)
				attron(A_REVERSE);
			attron(COLOR_PAIR(1));

			mvprintw(i-selection + (y_max/2), 0, "%s", options[i]);

			if(i == selection)
				attroff(A_REVERSE);
			attroff(COLOR_PAIR(1));
		}

		refresh();
		ch = getch();

		switch(ch)
		{
		    case KEY_RESIZE:
				break;

			case KEY_ETX:
			case KEY_EOT:
				strcpy(selection_s, "");
				endwin();
				printf("EOF/EOT received\n");
				return -1;


			case KEY_UP:
			case KEY_LEFT:
				selection = (selection - 1 + n_of_options) % n_of_options; break;
			case KEY_DOWN:
			case KEY_RIGHT:
				selection = (selection + 1) % n_of_options; break;
			case '\n':
				endwin();
				break;

			default:
				break;
		}
	}

	strcpy(selection_s, options[selection]);

	return selection;
}

#define COMMAND_LIST_SIZE 30
void command_select(command_node *head, char *utype, char *username)
{
    int selection = 0;
    int i;
    int ch = 0;
   	WINDOW *left_win, *right_win;
    int y_max, x_max;

    command cmd_list[COMMAND_LIST_SIZE];
    int cmd_list_n;
    command_node* Node;

    Value varray_username[1];
    varray_username[0] = make_string(username);

    i = 0;
    Node = head;
    while(Node != NULL && i < COMMAND_LIST_SIZE)
    {
        cmd_list[i] = Node->f;
        i++;
        Node = Node->next;
    }
    cmd_list_n = i;

    if (cmd_list_n == 0)
    {
        fprintf(stderr, "Your current usertpye does not have any commands!\n");
        endwin();
        exit(1);
    }

    ncurses_init();

    init_color(10, 106*(1000/255), 90*(1000/255), 205*(1000/255));
    init_pair(2, COLOR_WHITE, 10);
    clear();
    while(1)
    {
	   	refresh(); /* resumes ncurses mode  */
        getmaxyx(stdscr, y_max, x_max);

        left_win = newwin(y_max, x_max/2, 0, 0);
        box(left_win, 0 ,0);
        keypad(left_win, 1);

        right_win = newwin(y_max, (x_max/2), 0, (x_max/2)+1);

        /*
        box(right_win, 0, 0);
         */

        /*
        mvwprintw(left_win, 0,0, "%c %d", ch, selection);
         */
        mvwprintw(left_win ,0, 0, "%s %s", username, utype);

        wbkgd(left_win, COLOR_PAIR(2));
        for(i=0; i<cmd_list_n; i++)
        {
            if (i-selection + (y_max/2) < 1) /* if its gonna cover the pretext, just skip */
				continue;

            if(i == selection)
				wattron(left_win, A_REVERSE);

            mvwprintw(left_win, i-selection + (y_max/2) , 1, "%s", cmd_list[i].alias);

            if(i == selection)
				wattroff(left_win, A_REVERSE);
        }


        wrefresh(left_win);

        mvwprintw(right_win, 1, 0, "Permission: %s", cmd_list[selection].permission);

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
                mvwprintw(right_win, i+3, 0, "Argument %d: %s", i, taip);
            }

            mvwprintw(right_win, i+4, 0, "Description:");
            mvwprintw(right_win, i+5, 0, "%s" ,cmd_list[selection].desc);
        }

        wrefresh(right_win);

        ch = wgetch(left_win);

        switch(ch)
        {
        	case KEY_ETX:
        		endwin();
       			printf("Received SIGINT, bye!\n");
        		return;
			case KEY_EOT:
				endwin();
				printf("Received EOT, bye!");
				return;

            case KEY_LEFT:
            case KEY_UP:
                selection = (selection - 1 + cmd_list_n) % cmd_list_n;
                break;
            case KEY_RIGHT:
            case KEY_DOWN:
                selection = (selection + 1) % cmd_list_n;
                break;

            case KEY_RESIZE:
           		clear();
                break;

            case '\n':
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
                    int n;

                    n = cmd_list[selection].n_of_args;
                    args = malloc(sizeof(Value) * n);

                    for(i=0; i<n; i++)
                    {
                        args[i].type = cmd_list[selection].arg_type[i];
                        printf("Argument %d:\n", i);
                        switch (args[i].type)
                        {
                            case TYPE_BOOL:
                            case TYPE_INT:
                                printf("Enter an integer\n");
                                scanf("\n%d", &args[i].data.as_int); break;

                            case TYPE_STRING:
                                printf("Enter a string\n");
                                scanf("\n%128s", args[i].data.as_string); break;

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

                    raw();
                    getch();

                }else
                {
                    call(cmd_list[selection], varray_username, 1, utype);

                    /*
                     *
                     * Sets terminal to raw mode to immediately consume character, (default is cooked mode aka dont stop until EOL)
                     * This way the program can immediately continue after calling the function pointer, without showing the user funny characters, or having to go back to ncurses mode and the user misses the output from the function
                     *
                     */
                    raw();
                    getch();
                }
                break;

            default:
                break;
        }
	    delwin(right_win);
	    delwin(left_win);
    }
}

time_t time_picker(char * pretext, int color_fg, int color_bg, time_t pre_time)
{
    int ch = 0;

    time_t time = pre_time;
    struct tm * ts;
    char readable_time[80];

    enum incr_range
    {
      YEAR,
      MONTH,
      DAY,
      HOUR,
      MINUTE,
      SECOND
    };
    int select = DAY;
    int change = 0;
    char range_s[7];


	ts = localtime(&time);


	ncurses_init();
	init_pair(2, color_fg, color_bg);

	attron(COLOR_PAIR(2));
	mvprintw(0, 0, "%s", pretext);
	attroff(COLOR_PAIR(2));

	while(ch != '\n')
	{
		refresh();
		switch(select)
		{
		    case YEAR:
				strcpy(range_s, "Year");
				break;
			case MONTH:
                strcpy(range_s, "Month");
    			break;
			case DAY:
                strcpy(range_s, "Day");
    			break;
			case HOUR:
                strcpy(range_s, "Hour");
    			break;
			case MINUTE:
                strcpy(range_s, "Minute");
    			break;
			case SECOND:
                strcpy(range_s, "Second");
    			break;

			default:
			break;
		}

		time = mktime(ts);
		strftime(readable_time, sizeof(readable_time),"%Y-%m-%d %H:%M:%S", ts);
  		mvprintw(1, 0, "%s", readable_time);

		attron(COLOR_PAIR(2));
            mvprintw(2, 0, "         "); /*funny clear*/

            attron(A_REVERSE);
            mvprintw(2, 0, "%s", range_s);
            attroff(A_REVERSE);
		attroff(COLOR_PAIR(2));


		refresh();

		ch = getch();


		switch(ch)
		{
			case KEY_ETX: /* ETX */
			case KEY_EOT: /* EOF */
				fprintf(stderr, "Received EOF/EOT, cancelling time picker");
				endwin(); return -1;

			case KEY_UP:
			    change = 1;
			    break;
			case KEY_DOWN:
			    change = -1;
			    break;

			case KEY_LEFT:
                select = (select - 1 + 6) % 6;
                change = 0;
                break;
			case KEY_RIGHT:
    			select = (select + 1) % 6;
                change = 0;
                break;


			case '\n':
			    change = 0;
				endwin();
				return time;

			case KEY_RESIZE:
				change = 0;
                break;

            default:
            	change = 0;
            	break;
		}

		switch(select)
		{
		    case YEAR:
				ts->tm_year += change;
				strcpy(range_s, "Year");
				break;
			case MONTH:
    			ts->tm_mon  += change;
                strcpy(range_s, "Month");
    			break;
			case DAY:
    			ts->tm_mday += change;
                strcpy(range_s, "Day");
    			break;
			case HOUR:
    			ts->tm_hour += change;
                strcpy(range_s, "Hour");
    			break;
			case MINUTE:
    			ts->tm_min  += change;
                strcpy(range_s, "Minute");
    			break;
			case SECOND:
    			ts->tm_sec  += change;
                strcpy(range_s, "Second");
    			break;

			default:
			break;
		}

	}
	return time;
}

#define MAX_BOOKING_SLOTS 128
int booking_slot_view(Slot * slots, int slots_n, const char * doctor, int patient)
{
    char readable_start[MAX_BOOKING_SLOTS][80];
    char readable_end[MAX_BOOKING_SLOTS][80];
    int i;

    time_t time;
    struct tm *time_tm;
    char buffer[80];
    char username[64];
    strncpy(username, current_username, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';


    if(slots_n == 0)
    {
        printf("No appointment slots available for %s\n", doctor);
        return -1;
    }

    for(i=0; i<slots_n && i < MAX_BOOKING_SLOTS; i++)
    {
        time = slots[i].start;
        time_tm = localtime(&time);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_tm);
        strcpy(readable_start[i], buffer);

        time = slots[i].end;
        time_tm = localtime(&time);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_tm);
        strcpy(readable_end[i], buffer);
    }



    {
       	WINDOW *left_win, *right_win;
        int selection = 0;
        int i;
        int ch = 0;
        int y_max, x_max;

        ncurses_init();
        init_color(10, 106*(1000/255), 90*(1000/255), 205*(1000/255));
        init_pair(2, COLOR_WHITE, 10);
        while(1)
        {
       		refresh();
            getmaxyx(stdscr, y_max, x_max);

            left_win = newwin(y_max, x_max/2, 0, 0);
            box(left_win, 0 ,0);
            keypad(left_win, 1);

            right_win = newwin(y_max, (x_max/2), 0, (x_max/2));

            mvwprintw(left_win, 0, 0, "%s's appointment slots", doctor);

            wbkgd(left_win, COLOR_PAIR(2));
            for(i=0; i<slots_n; i++)
            {
                if (i-selection + (y_max/2) < 1) /* if its gonna cover the pretext, just skip */
                    continue;

                if(i == selection)
				wattron(left_win, A_REVERSE);

                mvwprintw(left_win, i-selection + (y_max/2), 1, "%s %d", "slop", i);

                if(i == selection)
				wattroff(left_win, A_REVERSE | COLOR_PAIR(2));
            }
            wrefresh(left_win);

            if(patient)
            {
                /* if booked by me or is vacant, show details */
                if(strcmp(slots[selection].patient, username) == 0 OR strcmp(slots[selection].patient, "NULL") == 0)
                {
                    mvwprintw(right_win, 1, 1, "start: %s", readable_start[selection]);
                    mvwprintw(right_win, 2, 1, "end: %s", readable_end[selection]);
                    mvwprintw(right_win, 3, 1, "venue: %s", slots[selection].venue);
                }

                if(strcmp(slots[selection].patient, username) == 0)
                    mvwprintw(right_win, 5, 1, "Booked by you");

                else if(strcmp(slots[selection].patient, "NULL") == 0)
                    mvwprintw(right_win, 5, 1, "Vacant!");


            }else
            {
                mvwprintw(right_win, 1, 1, "start: %s", readable_start[selection]);
                mvwprintw(right_win, 2, 1, "end: %s", readable_end[selection]);
                mvwprintw(right_win, 3, 1, "venue: %s", slots[selection].venue);

                mvwprintw(right_win, 5, 1, "Patient: %s", slots[selection].patient);
            }

            wrefresh(right_win);



            ch = wgetch(left_win);

            switch (ch)
            {
	            case KEY_ETX: /* ETX */
				case KEY_EOT: /* EOF */
					delwin(right_win);
                    delwin(left_win);
					endwin();
					fprintf(stderr, "Received EOF/EOT, cancelled booking view\n");
					return -1;



                case KEY_LEFT:
                case KEY_UP:
                    selection = (selection - 1 + slots_n) % slots_n;
                    break;
                case KEY_RIGHT:
                case KEY_DOWN:
                    selection = (selection + 1) % slots_n;
                    break;
                case KEY_RESIZE:
                    break;

                case '\n':
                    delwin(right_win);
                    delwin(left_win);
                    endwin();

                    if(patient)
                    {
                        if(strcmp(slots[selection].patient, "NULL") == 0 OR strcmp(slots[selection].patient, username) == 0)
                            return selection;
                    }
                    else
                        return selection;
            }
        }

    }

}

int config_editor(Config config, const char * path, const char * const * disallow, int disallow_n)
{
	int ch, y_max, x_max, selection = 0;
	int i;
	int edit_mode = 0;
	int cur_pos_x;
	WINDOW *left_win, *right_win;
	char edit_buffer[MAX_CONFIG_VAL_LEN];
	int buffer_max;
	int allow = 1;


	ncurses_init();
	init_pair(1, COLOR_BLACK, COLOR_RED);

	clear();
	getmaxyx(stdscr, y_max, x_max);
	while(1)
	{
		refresh();

		bkgd(COLOR_BLACK);

		if(edit_mode)
		{
			mvprintw(0, 0, "EDIT MODE  ");
			curs_set(1);
			mvprintw(y_max/2, (x_max/4), "                                                     "); /*goofy ahh clear */
			mvprintw(y_max/2, (x_max/4) + 1, "%s", edit_buffer);
			move(y_max/2, cur_pos_x + (x_max/4) + 1);
		}
		else
		{
			mvprintw(0, 0, "SELECT MODE");

			curs_set(0);

			left_win = newwin(y_max, (x_max/4), 0, 0);
			box(left_win, 0, 0);
			keypad(left_win, 1);

			right_win = newwin(y_max, (3*x_max/4), 0, (x_max/4) + 1);
			mvwprintw(right_win, 0, 0, "CTRL+C/CTRL+D to exit without saving, CTRL+W to save.\nEnter to select a field to edit\nIn edit mode, you can use arrow keys and backspace or del key");
			for(i=0; i<config.count; i++)
			{
				if (i-selection + (y_max/2) < 1) /* if its gonna cover the pretext, just skip */
					continue;

				if(i == selection)
					wattron(left_win, A_REVERSE);

				mvwprintw(left_win, i-selection + (y_max/2), 1, "%s", config.entries[i].key);

				if(i == selection)
					wattroff(left_win, A_REVERSE);
			}
			wrefresh(left_win);

			if(allow)
			{
				mvwprintw(right_win, y_max/2, 0, "%s", config.entries[selection].value);
			}else
			{
				wattron(right_win, COLOR_PAIR(1));
				mvwprintw(right_win, y_max/2, 0, "You are not allowed to change this");
				wattroff(right_win, COLOR_PAIR(1));

				allow = 1;
			}
			wrefresh(right_win);
		}

		ch = getch();


		switch (ch)
		{
			case KEY_RESIZE:
				clear();
				getmaxyx(stdscr, y_max, x_max); break;

			case KEY_ETX:
			case KEY_EOT: /* EOF */
				endwin();
				fprintf(stderr, "config_editor(): Received EOF/EOT, discarded changes\n");
				return -1;

			default: break;
		}

		if(edit_mode == 1)
			switch (ch)
			{
				case KEY_HOME:
					cur_pos_x = 0;
					break;

				case KEY_END:
					cur_pos_x = buffer_max;
					break;

				case KEY_LEFT:
					if(cur_pos_x > 0)
						cur_pos_x--;
					break;

				case KEY_RIGHT:
					if(cur_pos_x < buffer_max + 1)
						cur_pos_x++;
					break;

				case KEY_BACKSPACE:
					if(cur_pos_x > 0)
					{
						string_pop(edit_buffer, cur_pos_x-1);
						cur_pos_x--;
						buffer_max--;
					}
					break;

				case KEY_DC: /* delete key */
					if(cur_pos_x < buffer_max + 1)
					{
						string_pop(edit_buffer, cur_pos_x);
						buffer_max--;
						if(cur_pos_x > buffer_max)
							cur_pos_x = buffer_max + 1;
					}
					break;

				case '\n':
					edit_mode = 0;
					strcpy(config.entries[selection].value, edit_buffer);

					delwin(right_win);
					right_win = NULL;
					delwin(left_win);
					left_win = NULL;

					break;

				default:
					if(ch >= 32 AND ch <= 126) /* printable ASCII */
					{
						buffer_max++;
						if(buffer_max > MAX_CONFIG_VAL_LEN)
						{
							mvprintw(y_max, 0, "Warning: character length limit");
							buffer_max--;
							break;
						}
						string_insert(edit_buffer, ch, cur_pos_x);
						cur_pos_x++;
					}
					break;;
			}
		else
			switch (ch)
			{
				case KEY_CTRL_W: /* CTRL+W */
					endwin();
					printf("Saving to %s\n", path);
					dump_config_to_file(config, path);
					return 0;

				case KEY_LEFT:
	            case KEY_UP:
	                selection = (selection - 1 + config.count) % config.count;
	                break;
	            case KEY_RIGHT:
	            case KEY_DOWN:
	                selection = (selection + 1) % config.count;
	                break;

				case '\n':
					allow = 1;
					for(i=0; i<disallow_n; i++)
						if(strcmp(disallow[i], config.entries[selection].key) == 0)
						{
							allow = 0;
							break;
						}

					if(!allow)
					{
						break;
					}

					edit_mode = 1;
					strcpy(edit_buffer, config.entries[selection].value); /* value is null terminated during parsing */

					cur_pos_x = strlen(edit_buffer);
					buffer_max = cur_pos_x;
					break;


	            default: break;
			}

		if(right_win != NULL)
		{
			delwin(right_win);
			right_win = NULL;
		}

		if(left_win != NULL)
		{
			delwin(left_win);
			left_win = NULL;
		}
	}
}

int text_editor(const char * path, int readonly)
{
	int lines_n, current_n, i, ch;
	int y_offset = 0, x_offset = 0, cur_x = 0, cur_y = 0;
	int x_max, y_max;
	int line_number_offset=0, temp;

	char ** lines = file_to_strings(path, &lines_n);

	if (lines == NULL)
		return -1;

	/* count digits of n to get offset, so i can print the line number on left side */
	temp = lines_n;
	if(lines_n == 0)
		line_number_offset = 1;
	else
		while(temp != 0)
		{
			temp /= 10;
			line_number_offset++;
		}

	ncurses_init();
	getmaxyx(stdscr, y_max, x_max);
	clear();
	current_n = strlen(lines[0]);
	while(1)
	{
		refresh();
		curs_set(1);

		for(i=0; i<y_max-1; i++) /* bottom line is reserved for instructions */
		{
			int ii;
			int length;
			if( (i+y_offset)>=lines_n OR (i+y_offset)<0 ) /* prevent sigsegv */
			{
				continue;
			}

			length = strlen(lines[i+y_offset]);

			if(length>=x_max - 1) /* if length exceeds terminal size, replace last char with '>'. Like a linux terminal pager */
			{
				for(ii=0; ii<x_max+line_number_offset-1; ii++)
					mvaddch(i, ii+line_number_offset + 1, lines[i+y_offset][ii]);
				mvaddch(i, x_max-1, '>');
			}else
			{
				mvprintw(i, line_number_offset + 1, "%s", lines[i+y_offset]);
			}
		}
		move(cur_y, cur_x);

		ch = getch();
		switch(ch)
		{
			case KEY_RESIZE:
				getmaxyx(stdscr, y_max, x_max);
				clear();
			break;

			case KEY_ETX:
				endwin();
				printf("text_editor() SIGINT Received, discarded changes\n");
			return -1;

			case KEY_EOT:
				endwin();
				printf("text_editor() EOF Received, discarded changes\n");
			return -1;

			case KEY_UP:
				if(cur_y > 0)
				{
					cur_y--;
					current_n = strlen(lines[cur_y]);

					if(cur_x > (current_n + line_number_offset))
						cur_x = current_n + line_number_offset + 1;
				}
			break;

			case KEY_DOWN:
				if(cur_y < y_max-1 && cur_y < lines_n-1)
				{
					cur_y++;
					current_n = strlen(lines[cur_y]);

					if(cur_x > (current_n + line_number_offset))
						cur_x = current_n + line_number_offset + 1;
				}
			break;

			case KEY_LEFT:
				if(cur_x > 0)
					cur_x--;
			break;

			case KEY_RIGHT:
				if(cur_x < x_max && cur_x <= (current_n + line_number_offset))
					cur_x++;
			break;

			if(!readonly)
			{
				default:
					if(ch >= 32 AND ch <= 126) /* printable ASCII */
					{
						current_n++;
						string_insert(lines[i], ch, cur_x+line_number_offset);
						cur_x++;
					}
					break;;
			}
		}


	}




	for(i=0; i<lines_n; i++)
	{
		free(lines[i]);
	}
	free(lines);
	return 0;
}
