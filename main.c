#include <stdio.h>
#include <ctype.h>
#include "logins.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

/* file handling */
#include <sys/stat.h>
#include <errno.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

/* password hashing */
#include "deps/crypt_blowfish/ow-crypt.h"


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
	user_login_t* logins= NULL;

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
		case 0: printf("fee"); break;

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

			if (stat(path, &st) == -1)
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
                    {
                        char entropy[18];
                        char salt[64];
                        char *hashed_password;

                        #ifdef _WIN32
                        HCRYPTPROV hProvider = 0;
                        if (CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
                            CryptGenRandom(hProvider, sizeof(entropy), (BYTE*)entropy);
                            CryptReleaseContext(hProvider, 0);
                        } else {
                            strcpy(entropy, "0123456789abcdef"); /* fallback */
                        }
                        #else
                        int urandom_fd = open("/dev/urandom", O_RDONLY);
                        if (urandom_fd != -1) {
                            read(urandom_fd, entropy, sizeof(entropy));
                            close(urandom_fd);
                        } else {
                            strcpy(entropy, "0123456789abcdef"); /* fallback */
                        }
                        #endif

                        crypt_gensalt_rn("$2a$", 12, entropy, sizeof(entropy), salt, sizeof(salt));
                        hashed_password = crypt(password, salt);
                        if (hashed_password != NULL) {
                            strcpy(password, hashed_password);
                        }
                    }

                    logins = NULL;


                    pw_fd = open(pw_file, O_RDWR | O_CREAT, 0666);
                    if (pw_fd != -1)
                    {
                        flock(pw_fd, LOCK_EX);
                    }
                    load_users_from_file(pw_file, &logins);
                    add_user(current_username, password, &logins);
                    save_users_to_file(pw_file, &logins);

                    if (pw_fd != -1)
                    {
                        flock(pw_fd, LOCK_UN);
                        close(pw_fd);
                    }


                    free(password);
                    build_patient(current_username);
                }else
                {
                    perror("Error checking dir\n");
                    strcpy(pretext, "Error checking dir");
                }
		    } else
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
