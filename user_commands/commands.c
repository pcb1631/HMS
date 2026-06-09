#include "commands.h"
#include "../utils/utils.h"
#include "../utils/logins.h"
#include "../utils/tui.h"
#include "../utils/globals.h"
#include "../utils/string_utils.h"
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #include <ncursesw/ncurses.h>
	#include "../utils/win_api.h"
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
    char path[256];
    user_select_menu(allowed, 2, username);

    if(strcmp(username, "")==0)
    	return make_string("EOF/EOT");

    sprintf(path, "./users/%s/profile.txt", username);
    parse_config_file(path, &user);

    printf("\n%s:\n", username);
    print_cfg(user);
    return make_string("OK");
}

Value director_view_cfg(void)
{
    Config user;
    const char * allowed[] =
        {
            "Doctor",
            "Nurse",
            "Admin",
        };

    char username[MAX_USERNAME_LEN];
    char path[256];
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

Value edit_profile_self_patient(void)
{
	char path[256];
	char username[MAX_USERNAME_LEN];
	const char* disallow[] = {"USERTYPE", "blood_type", "height"};
	Config profile;

	strcpy(username, current_username);
	sprintf(path, "./users/%s/profile.txt", username);

	parse_config_file(path, &profile);

	return make_int(config_editor(profile, path, disallow, sizeof(disallow)/sizeof(char *)));
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

Value admin_delete_user(void)
{
	user_login_t * users = NULL;
	char username[MAX_USERNAME_LEN];
	char path[512];
	const char * allowed_utypes[] = 
		{
			"Doctor",
			"Patient",
			"Nurse"
		};
	
	user_select_menu(allowed_utypes, 3, username);

	sprintf(path, "%s/%s", DATABASE_PATH, "creds.txt");
	load_users_from_file(path, &users);
	if(lookup_user(username, &users) == NULL)
	{
		printf("%s not found\n", username);
		return make_int(-1);
	}
	else
	{
		delete_user(username, &users);
	}
	save_users_to_file(path, &users);
	
	
	#ifdef _WIN32		
	sprintf(path, "%s/%s", DATABASE_PATH, username);
	if(rm_rf_windows(path) != 0)
	{
		perror("admin_delete_user()");
		return make_int(-1);
	}
	
	#else
		sprintf(path, "%s/%s", DATABASE_PATH, username);
		rm_rf(path);
	#endif

	printf("Removed %s\n", username);
	
	return make_int(0);
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

#include <sys/stat.h>
Value doctor_diagnosis(void)
{
	const char * allowed[] = {"Patient", };
	char filename[256];
	char path[4096];
	char patient[MAX_USERNAME_LEN];
	char doctor[MAX_USERNAME_LEN];
	char readable_time[80];
	FILE * file;

	time_t now = time(NULL);
	struct tm * ts = localtime(&now);
	 
	strcpy(doctor, current_username);
	user_select_menu(allowed, 1, patient);
	
	strftime(readable_time, sizeof(readable_time),"%Y-%m-%d_%H_%M_%S", ts);
	

	/* make a folder for every doctor in the diag report dir, so doctors are only allowed to view their own diags */
	sprintf(path, "./users/%s/diag_report/%s", patient, doctor); 
	#ifdef _WIN32
		mkdir(path);
	#else
		mkdir(path, 0755);
	#endif

	printf("Enter filename (or leave empty to just use current time)\n");
	fgets(filename, 256, stdin);
	string_trim(filename);

	if(isempty(filename))
		sprintf(path, "./users/%s/diag_report/%s/diag_%s.txt", patient, doctor, readable_time);
	else
		sprintf(path, "./users/%s/diag_report/%s/%s.txt", patient, doctor, filename);

	if(!path_exists(path)) /* if the file exists, do not overwrit e */
	{
		file = fopen(path, "w");
		if(file == NULL)
		{
			perror("doctor_diagnosis()");
			return make_int(-1);
		}
	
		fprintf(file, "Doctor: %s %s", doctor, readable_time);
		fclose(file);
	}

	text_editor(path, 0);
	
	return make_int(0);
}

Value view_diagnosis_self_doc(void)
{
	const char * allowed[] = {"Patient", };
	const char ** files;
	int files_n = 0;
	char file[256];
	char patient[MAX_USERNAME_LEN];
	char doctor[MAX_USERNAME_LEN];
	char path[4096];
	int i;
	
	user_select_menu(allowed, 1, patient);
	strcpy(doctor, current_username);
	sprintf(path, "./users/%s/diag_report/%s/", patient, doctor);

	files = (const char **)list_files_indir(path, &files_n);
	if(files == NULL)
	{
		perror("view_diagnosis_self_doc()");
		printf("attempted to open: %s\n", path);
		return make_int(-1);
	}

	clear();

	/* keep letting user choose files until EOF/EOT */
	while(simple_select(COLOR_YELLOW, COLOR_BLACK, files_n, files, file, "Diagnosis reports: ") != -1) 
	{
		sprintf(path, "./users/%s/diag_report/%s/%s", patient, doctor, file);
		clear();
		text_editor(path, 1);
		clear();
	}
	

	for(i=0; i<files_n; i++)
		free((void*) files[i]);
	free(files);

	return make_int(0);
}