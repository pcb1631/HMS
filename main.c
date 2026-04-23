#include <stdio.h>
#include "logins.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

#include "commands.h"

/* file handling */
#include <sys/stat.h>
#include <errno.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


/* TUI */
#include "tui.h"
#ifndef _WIN32
	#include <ncurses.h>
#else
	#include <ncursesw/curses.h>
	#include <windows.h>
	#include <wincrypt.h>
#endif

int main(void)
{
	struct stat st;
	char path[256];

	char * pw_file = "./users/creds.txt";
	char * options[3] = {
		"Login\0",
		"Register\0",
		"Exit\0"
	};
	char pretext[128] = "Please select an option";
	char selection[20] = "";

	char current_username[64];
	char current_utype[10];

	/*Pass to logins.h functions as a pointer to hash pointer, as per uthash doc

	Defined in main because global variables aren't allowed */
	user_login_t * logins = NULL;
	user_login_t * user_ptr = NULL;

	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_make_user[] = {TYPE_STRING, TYPE_STRING};

	/* COMMAND(function name, alias, permission, argument types) */
	static command cmd_list[] = {
		COMMAND(add, "add", "Patient", args_add, "Add two numbers"),
		COMMAND_NOARGS(meow, "meow", "Doctor", "Meow"),
		COMMAND_NOARGS(cat, "cat", "Cats", "Placeholder"),
		COMMAND(make_user, "Register users", "Admin", args_make_user, "First arg is user type, second is username")
	};

	int cmd_list_n = sizeof(cmd_list)/sizeof(command);




	main_menu:

	switch (simple_select(COLOR_BLUE, COLOR_BLACK, 3, options, selection, pretext))
	{
	    /* login */
		case 0:

        login_get_username:
        printf("enter your username\n");

        fgets(current_username, sizeof(current_username)/sizeof(char), stdin);
        if(!string_trim(current_username))
        {
            fprintf(stderr, "Username cannot be empty or contain spaces\n");
            goto login_get_username;
        }

        login_get_password:
	    {
			char * password;
            password = (char*)malloc(sizeof(char)*256);
            printf("enter your password\n");

            fgets(password, 256, stdin);
            if(!string_trim(password))
            {
                fprintf(stderr, "Password cannot be empty or contain spaces\n");
                free(password);
                goto login_get_password;
            }

            load_users_from_file(pw_file, &logins);
            user_ptr = lookup_user(current_username, &logins);

            if(user_ptr == NULL)
            {
                fprintf(stderr, "Wrong username or password\n");
                free(password);
                goto login_get_username;
            }

            printf("\033c");

            if(verify_password(password, user_ptr->password))
                printf("Password hash matched\n");
            else
            {
                printf("Wrong username or password\n");
                goto login_get_username;
            }

            free(password);
		}break;

		/* register */
		case 1:

		    reg_get_username:
			printf("enter your desired username\n");
			fgets(current_username, sizeof(current_username)/sizeof(char), stdin);
			if(!string_trim(current_username))
			{
                fprintf(stderr, "Username cannot be empty or contain spaces\n");
                goto reg_get_username;
			}


			strcpy(path, "./users/");
			strcat(path, current_username);

			if (stat(path, &st) == -1) /* if dir doesn't exist */
      		{
        		if (errno == ENOENT)
                {
                    char * password;
                    /*check if username exists in hashtable*/
                    load_users_from_file(pw_file, &logins);
                    if(lookup_user(current_username, &logins) != NULL)
                    {
                        fprintf(stderr, "This username has been taken (hashtable)\n");
                        goto reg_get_username;
                    }


                    /* get password, save to hashtable, dealloc string */
                    reg_get_password:
                    password = (char*)malloc(sizeof(char)*256);
                    printf("enter your password\n");
                    fgets(password, 256, stdin);
                    if(!string_trim(password))
                    {
                        fprintf(stderr, "Password cannot be empty or contain spaces\n");
                        free(password);
                        goto reg_get_password;
                    }


                    /* hash the password */
                    register_user_hashtable(current_username, password, pw_file);
                }else
                {
                    perror("Error checking dir\n");
                    strcpy(pretext, "Error checking dir");
                }
		    }
			else /* if dir exists */
      		{
				if (S_ISDIR(st.st_mode))
				{
                    fprintf(stderr, "This username has been taken (dir exists)\n");
                    goto reg_get_username;
				} else
				{
					strcpy(pretext, "Path exists, but it's not a dir (please contact admin)");
					goto main_menu;
				}
		    }
			generate_file_structure("Patient", current_username);
            sprintf(pretext, "Registered %s as patient, please login again\n", current_username);
            goto main_menu;

		/* exit */
		case 2:
			return 0;
	}


	/* this entire block is command selector */
	{
	    int selection = 0;
	    int i;
	    int ch = 0;
       	WINDOW *left_win, *right_win;
        int y_max, x_max;
        Value placeholder[1];
        placeholder[0] = make_int(0);

        {
            Config cfg;
            const char *val;
            sprintf(path, "./users/%s/profile.txt", current_username);
            if (parse_config_file(path, &cfg) == 0) {
                val = get_config_value(&cfg, "USERTYPE");
                if (val != NULL) {
                    strncpy(current_utype, val, sizeof(current_utype) - 1);
                    current_utype[sizeof(current_utype) - 1] = '\0';
                }
            }
        }

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
            box(right_win, 0, 0);

            /*
            mvwprintw(left_win, 0,0, "%c %d", ch, selection);
            */

            mvwprintw(left_win ,0, 0, "%s %s", current_username, current_utype);

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

            /* display types and desc */
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

                    if(strcmp(current_utype, cmd_list[selection].permission) != 0)
                    {
                        printf("Permission denied (current perm: %s, command perm: %s", current_utype, cmd_list[selection].permission);
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
                                default:
                                    break;;
                            }
                            printf("\n");
                        }

                        call(cmd_list[selection], args, cmd_list[selection].n_of_args, current_utype);
                        free(args);
                        getchar();

                    }else
                    {
                        call(cmd_list[selection], placeholder, 1, current_utype);
                    }
                    getchar();
                    break;

                default:
                    break;
            }
        }
    }











    endwin();
	return 0;
}
