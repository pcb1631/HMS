#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_commands/complaints.h"
#include "utils/globals.h"
#include "utils/logins.h"
#include "utils/utils.h"
#include "utils/string_utils.h"
#include "user_commands/bookings.h"
#include "user_commands/commands.h"
#include "user_commands/inventory.h"
#include "utils/tui.h"
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #include <ncursesw/ncurses.h>
    #include <windows.h>
    #include <wincrypt.h>
#else
    #include <sys/file.h>
    #include <unistd.h>
    #include <ncurses.h>
#endif

#define CREDENTIALS_PATH "./users/creds.txt"

int main(int argc, char ** argv)
{
    char current_utype[10];
    char * pw_file = CREDENTIALS_PATH;
	char path[256];
	const char * options[3] = {
		"Login\0",
		"Register\0",
		"Exit\0"
	};
	char pretext[128] = "Please select an option";
	char selection[20] = "";


	/*Pass to logins.h functions as a pointer to hash pointer, as per uthash doc

	Defined in main because global variables aren't allowed */
	user_login_t * logins = NULL;
	user_login_t * user_ptr = NULL;


	/* i have 0 clue why const doesnt work
	 * if i don't define with static, clangd will scream something about aggregate not being a compile time constant
	 */
	static ValueType makeuser_args[] = {TYPE_STRING, TYPE_STRING};


	/* COMMAND(function name, alias, permission, argument types) */
	const command cmd_list[] = {
        COMMAND_NOARGS(doctor_booking_frontend, "Doctor Booking", "Doctor", "Booking options for doctors"),
        COMMAND_NOARGS(doctor_diagnosis, "Write diagnosis", "Doctor", "Create a diagnosis report for a patient"),
        COMMAND_NOARGS(view_diagnosis_self_doc, "View diagnosis", "Doctor", "View diagnosis reports for a patient you wrote yrself"),

        
        COMMAND_NOARGS(patient_booking_frontend, "Patient Booking", "Patient", "Booking options for patients"),
        COMMAND_NOARGS(edit_profile_self_patient, "Edit your profile", "Patient", ""),
        COMMAND_NOARGS(create_complaint, "Create complaint", "Patient", "Spill the tea sis"),

        COMMAND_NOARGS(admin_view_cfg, "View users profile", "Admin", ""),
        COMMAND(make_user, "Create account", "Admin", makeuser_args, "Create accounts for patients, doctors, etc\n\n First arg is username,\n second arg is usertype (cAsE sEnSiTiVe)"),
        COMMAND_NOARGS(edit_profile_self_admin, "Edit your profile", "Admin", ""),
        COMMAND_NOARGS(tedit_test, "Text edit test", "Admin", ""),
        COMMAND_NOARGS(admin_delete_user, "Delete user", "Admin", "Delete a user from cred file and DB folder"),
        	
        COMMAND_NOARGS(nurse_view_booking, "View doctor's appointments", "Nurse", ""),
        COMMAND_NOARGS(inventory_edit, "View and edit inventory", "Nurse", "View and edit fields for medical supplies and inventories"),
        COMMAND_NOARGS(inventory_edit_txteditor, "Edit inventory with text editor", "Nurse", ""),
	};

	int cmd_list_n = sizeof(cmd_list)/sizeof(command);

	/* in case anyone is stupid enough to run the program by typing the full path instead of cding in
	 * the first argv is always the path used to run main.out;
		* On windows, it's always the full path
		* For example on linux, if you run it by doing ./main.out, it will be exactly that. if you run it by /home/username/github/main.out, it will also be that.
	 */
    char argv_path[256];
    char * last_slash;


    strcpy(argv_path, argv[0]);
    last_slash = strrchr(argv_path, '/');

    if (last_slash != NULL)
    {
        *last_slash = '\0';
    }

	#ifdef _WIN32
        _chdir(argv_path);
	#else
        chdir(argv_path);
        if(isatty(STDOUT_FILENO))
       		system("reset"); /* in case the program crashes and messes up the terminal */
    #endif

	if(argc > 1)
	{
        if ( strcmp(argv[1], "login") == 0 )
            if(argc > 2)
            {
                strcpy(current_username, argv[2]);
                goto login_get_password;
            }

            else
                goto login_get_username;


        else if ( strcmp(argv[1], "register") == 0 )
    	    goto reg_get_username;
	}





	main_menu:

	switch (simple_select(COLOR_BLUE, COLOR_BLACK, 3, options, selection, pretext))
	{
		case -1: /* EOT or EOF */
			return 0;
	    /* login */
		case 0:

        login_get_username:
        printf("enter your username\n");

        fgets(current_username, sizeof(current_username)/sizeof(char), stdin);
        string_trim(current_username);

        login_get_password:
	    {
			char password[MAX_PASSWORD_LEN];
            printf("enter your password\n");

            fgets(password, MAX_PASSWORD_LEN, stdin);
            string_trim(password);

            if(load_users_from_file(pw_file, &logins) == -1)
            {
                printf("Failed to load users from pw file");
                return -1;
            }
            user_ptr = lookup_user(current_username, &logins);

            if(user_ptr == NULL)
            {
                fprintf(stderr, "Wrong username or password\n");
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
		}break;

		/* register */
		case 1:

		    reg_get_username:
			printf("enter your desired username\n");
			fgets(current_username, sizeof(current_username)/sizeof(char), stdin);
			string_trim(current_username);


			strcpy(path, DATABASE_PATH);
			strcat(path, current_username);

			if (!path_exists(path)) /* if dir doesn't exist */
      		{
                char password[MAX_PASSWORD_LEN];
                /*check if username exists in hashtable*/
                load_users_from_file(pw_file, &logins);
                if(lookup_user(current_username, &logins) != NULL)
                {
                    fprintf(stderr, "This username has been taken (hashtable)\n");
                    goto reg_get_username;
                }


                /* get password, save to hashtable, dealloc string */
                printf("enter your password\n");
                fgets(password, MAX_PASSWORD_LEN, stdin);
                string_trim(password);

                /* hash the password */
                if(register_user_hashtable(current_username, password, pw_file) == -1)
                {
                    sprintf(pretext, "Failed to register user to hashtable");
                    goto main_menu;   
                }
				generate_file_structure("Patient", current_username);
	            sprintf(pretext, "Registered %s as patient, please login again\n", current_username);
	            goto main_menu;
		    }
			else /* if dir exists */
      		{
                fprintf(stderr, "This username has been taken (dir exists)\n");
                goto reg_get_username;
		    }

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
            }else
            {
                fprintf(stderr, "Usertype is empty\n");
                return 1;
            }
        }
    }

    {
        int i;
        int first = 1;
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

        command_select(head, current_utype, current_username);

        free_cmd_list(head);
    }







    endwin();
	return 0;
}
