#include <stdio.h>
#include "bookings.h"
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

	ValueType args_add[] = {TYPE_INT, TYPE_INT};
	ValueType args_make_user[] = {TYPE_STRING, TYPE_STRING};

	/* COMMAND(function name, alias, permission, argument types) */
	command cmd_list[] = {
        COMMAND_NOARGS(doctor_booking_frontend, "Doctor Booking", "Doctor", "Booking options for doctors"),
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

    {
        Config cfg;
        const char *val;
        sprintf(path, "./users/%s/profile.txt", current_username);
        if (parse_config_file(path, &cfg) == 0)
        {
            val = get_config_value(&cfg, "USERTYPE");
            if (val != NULL)
            {
                strncpy(current_utype, val, sizeof(current_utype) - 1);
                current_utype[sizeof(current_utype) - 1] = '\0';
            }
        }
    }

    {
        int i;
        bool first = 1;
        command_node* head;
        command_node* Node;

        for(i=0; i<cmd_list_n; i++)
        {
            if( strcmp(cmd_list[i].permission, current_utype) == 0 )
            {
                if(first)
                {
                    head = new_command_node(cmd_list[i]);
                    Node = head;
                    first = 0;
                    continue;
                }

                Node->next = new_command_node(cmd_list[i]);
                Node = Node->next;
            }
        }


        Node = head;
        printf("utype: %s\n", current_utype);
        while(Node != NULL)
        {
            printf("%s\n", Node->f.alias);
            Node = Node->next;
        }


        command_select(head, current_utype, current_username);

        free_cmd_list(head);
    }







    endwin();
	return 0;
}
