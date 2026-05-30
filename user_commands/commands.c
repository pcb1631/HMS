#include "commands.h"
#include "../utils/utils.h"
#include "../utils/logins.h"
#include "../utils/tui.h"
#include "../utils/globals.h"
#include "../utils/string_utils.h"

#ifdef _WIN32
    #include <ncursesw/curses.h>
#else
    #include <ncurses.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void test(void)
{
    printf("Hello, world!\n");
}

Value make_user(Value * list)
{
    char * username = list[0].data.as_string;
    char * utype    = list[1].data.as_string;

    int i=0;
    int flag=0;
    user_login_t * logins = NULL;
    char * pw_file = "./users/creds.txt";
    char pw[MAX_PASSWORD_LEN];

    char * allowed_utypes[4] =
    {
        "Admin",
        "Patient",
        "Nurse",
        "Doctor",
    };

    for(i=0; i<4; i++)
        if(strcmp(allowed_utypes[i], utype) == 0)
            flag=1;
    if(!flag)
    {
        printf("Usertype not allowed\n");
        return make_int(-1);
    }

    printf("Password: ");
    fgets(pw, MAX_PASSWORD_LEN, stdin);
    printf("\n");

    string_trim(pw);




    load_users_from_file(pw_file, &logins);
    register_user_hashtable(username, pw, pw_file);


    generate_file_structure(utype, username);
    return make_int(0);
}

Value admin_view_cfg(void)
{
    Config user;
    const char * allowed[] =
        {
            "Doctor",
            "Nurse"
        };

    char username[MAX_USERNAME_LEN];
    char path[128];
    user_select_menu(allowed, 2, username);

    if(strcmp(username, "")==0)
    	return make_string("EOF/EOT");

    sprintf(path, "./users/%s/profile.txt", username);
    parse_config_file(path, &user);

    printf("\n%s:\n", username);
    print_cfg(user);
    return make_string("OK");
}

void user_select_menu(const char * const * allowed_utypes, int allowed_n, char * selected_username)
{
    Config users;
    char ** options;
    const char ** options_const;
    int options_n = 0;
    int i;
    int ii;
    char utype[20];
    parse_config_file(PW_FILE, &users);

    options = (char **) malloc(sizeof(char *) * users.count);
    for(i=0; i<users.count; i++)
    {
        options[i] = (char *) malloc(sizeof(char) * 64);
    }



    for(i=0; i<users.count; i++)
    {
        char * username = users.entries[i].key;
        strcpy(utype, get_utype(users.entries[i].key));

        for(ii=0; ii<allowed_n; ii++)
            if(strcmp(utype, allowed_utypes[ii]) == 0)
            {
                strcpy(options[options_n], username);
                options_n++;
            }
    }

    options_const = (const char **)options;

    clear();
    simple_select(COLOR_BLUE, COLOR_BLACK, options_n, options_const, selected_username, "Select a user");

    for(i=0; i<users.count; i++)
       free(options[i]);
    free(options);
}

Value edit_profile_self_admin(void)
{
	char path[256];
	char username[MAX_USERNAME_LEN];
	const char* disallow[] = {"USERTYPE", };
	Config profile;

	strcpy(username, current_username);
	sprintf(path, "./users/%s/profile.txt", username);



	parse_config_file(path, &profile);

	return make_int(config_editor(profile, path, disallow, 1));
}


void print_cfg(Config cfg)
{
    int i;
    for(i=0; i<cfg.count; i++)
        printf("%s: %s\n", cfg.entries[i].key, cfg.entries[i].value);
    printf("\n");
}

Value tedit_test(void)
{
	char username[MAX_USERNAME_LEN];
	char path[256] = "";
	strcpy(username, current_username);
	sprintf(path, "temp1.txt");

	return make_int(text_editor(path, 0));
}
