#include <stdio.h>
#include "logins.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

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
	#include <ncurses/curses.h>
	#include <windows.h>
	#include <wincrypt.h>
#endif

int main(void)
{
	int i;

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

	/*Pass to logins.h functions as a pointer to hash pointer, as per uthash doc

	Defined in main because global variables aren't allowed */
	user_login_t * logins = NULL;
	user_login_t * user_ptr = NULL;
    char * hash;

	static ValueType args_add[] = {TYPE_INT, TYPE_INT};
	static ValueType args_meow[] = {TYPE_VOID};

	/* COMMAND(function name, alias, permission, argument types) */
	static command cmd_list[] = {
		COMMAND(add, "add", "debug", args_add),
		COMMAND(meow, "meow", "debug", args_meow),
	};




	main_menu:

	switch (simple_select(COLOR_MAGENTA, COLOR_BLACK, 3, options, selection, pretext))
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

            if(verify_password(password, user_ptr->password))
                printf("Password hash matched");
            else
                printf("Password hash not match");

            free(password);
		}

        break;

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
                    int pw_fd;
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
                    hash = (char*)malloc(256 * sizeof(char));

                    if(hash_password(password, hash) == -1)
                    {
                        free(hash);
                        fprintf(stderr, "Failed to hash password\n");
                        return 1;
                    }

                    logins = NULL;

                    pw_fd = open(pw_file, O_RDWR | O_CREAT, 0666);
                    if (pw_fd != -1)
                    {
                        #ifdef _WIN32

                            HANDLE hFile = (HANDLE)_get_osfhandle(pw_fd);
                            LockFile(hFile, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);

                        #else

                            flock(pw_fd, LOCK_EX);

                        #endif
                    }

                    load_users_from_file(pw_file, &logins);
                    add_user(current_username, hash, &logins);
                    save_users_to_file(pw_file, &logins);

                    if (pw_fd != -1)
                    {
                        #ifdef _WIN32
                            HANDLE hFile = (HANDLE)_get_osfhandle(pw_fd);
                            UnlockFile(hFile, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
                        #else
                            flock(pw_fd, LOCK_UN);
                        #endif

                        close(pw_fd);
                    }


                    free(password);
                    free(hash);
                    build_patient(current_username);
                }else
                {
                    perror("Error checking dir\n");
                    strcpy(pretext, "Error checking dir");
                }
		    } else /* if dir exists */
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
      		build_patient(current_username);
            sprintf(pretext, "Registered %s as patient, please login again\n", current_username);
            goto main_menu;

		/* exit */
		case 2:
			return 0;
	}


	return 0;
}
