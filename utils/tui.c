#include "tui.h"
#include "globals.h"
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

			case 0x03: /* ETX */
			case 0x04: /* EOF */
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
    while(1)
    {
	   	refresh(); /* resumes ncurses mode  */
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
        	case 0x03: /* ETX */
        		endwin();
       			printf("Received SIGINT, bye!\n");
        		return;
			case 0x04: /* EOT */
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
			case 0x03: /* ETX */
			case 0x04: /* EOF */
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
	            case 0x03: /* ETX */
				case 0x04: /* EOF */
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

int config_editor(Config config, const char * path)
{
	int ch, y_max, x_max, selection = 0;
	int i;
	int edit_mode = 0;
	int cur_pos_x;
	WINDOW *left_win, *right_win;
	char edit_buffer[MAX_CONFIG_VAL_LEN];
	int buffer_max;


	ncurses_init();
	getmaxyx(stdscr, y_max, x_max);
	while(1)
	{
		refresh();

		bkgd(COLOR_BLACK);

		if(edit_mode)
		{
			curs_set(1);
			clear();
			mvprintw(0, 0, "%s", config.entries[selection].key);
			mvprintw(1, 0, "%s", edit_buffer);
			move(1, cur_pos_x);
		}
		else
		{
			curs_set(0);

			left_win = newwin(y_max, x_max/2, 0, 0);
			box(left_win, 0, 0);
			keypad(left_win, 1);

			right_win = newwin(y_max, (x_max/2), 0, (x_max/2));
			mvwprintw(right_win, 0, 1, "CTRL+C/CTRL+D to exit without saving, CTRL+W to save. Enter to select a field to edit");

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

			mvwprintw(right_win, y_max/2, 1, "%s", config.entries[selection].value);
			wrefresh(right_win);



		}

		ch = getch();


		switch (ch)
		{
			case KEY_RESIZE:
				getmaxyx(stdscr, y_max, x_max); break;

			case 0x03: /* ETX */
			case 0x04: /* EOF */
				endwin();
				fprintf(stderr, "config_editor(): Received EOF/EOT, discarded changes\n");
				return -1;

			default: break;
		}

		if(edit_mode)
			switch (ch)
			{
				case KEY_LEFT:
					if(cur_pos_x > 0)
						cur_pos_x--;
					break;

				case KEY_RIGHT:
					if(cur_pos_x < buffer_max)
						cur_pos_x++;
					break;

				case KEY_BACKSPACE: /* replace current char with the char after, stop at /0 */
					if(cur_pos_x > 0)
					{
						i = cur_pos_x - 1;
						while(edit_buffer[i] != '\0')
						{
							edit_buffer[i] = edit_buffer[i+1];
							i++;
						}
						cur_pos_x--;
						buffer_max--;
					}
					break;

				case KEY_DC: /* delete key */
					i = cur_pos_x;
					while(edit_buffer[i] != '\0')
					{
						edit_buffer[i] = edit_buffer[i+1];
						i++;
					}

					if(cur_pos_x != 0)
						cur_pos_x--;


					buffer_max--;
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

						edit_buffer[buffer_max] = '\0';
						i = buffer_max - 1;
						while(cur_pos_x < i) /* replace current char with the char before */
						{
							edit_buffer[i] = edit_buffer[i-1];
							i--;
						}
						edit_buffer[cur_pos_x] = ch;
						cur_pos_x++;
					}
					break;;
			}
		else
			switch (ch)
			{
				case 0x17: /* CTRL+W */
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
