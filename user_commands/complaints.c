#ifdef _WIN32
	#include <ncursesw/ncurses.h>
#else
	#include <ncurses.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "complaints.h"
#include "../utils/utils.h"
#include "../utils/string_utils.h"
#include "../utils/globals.h"
#include "../utils/tui.h"
#include "commands.h"

Value create_complaint(void)
{
	FILE * file;
	char path[4096];
	char filename[256];
	char username[MAX_USERNAME_LEN];
	char readable_time[80];

	time_t now = time(NULL);
	struct tm *ts = localtime(&now);
	
	strcpy(username, current_username);
	strftime(readable_time, sizeof(readable_time), "%Y-%m-%d_%H_%M_%S", ts);

	printf("Enter file name, or leave empty to use current time\n");
	fgets(filename, 256, stdin);
	string_trim(filename);

	if(isempty(filename))
		sprintf(path, "%s/%s/complaints/%s.txt", DATABASE_PATH, username, readable_time);
	else
		sprintf(path, "%s/%s/complaints/%s.txt", DATABASE_PATH, username, filename);
		
	if(path_exists(path))
	{
		text_editor(path, 0);
	}
	else 
	{
		file = fopen(path, "w");
		if(file == NULL)
		{
			perror("create_complaint()");
			return make_int(-1);
		}
		fprintf(file, "%s %s", username, readable_time);
		fclose(file);

		text_editor(path, 0);
	}
	
	return make_int(0);
}

Value view_complaints(void) 
{
	Config users;
	Config profile;
	char patient[MAX_USERNAME_LEN];
	int i, ii, complaints_n;
	char ** files;
	char filename[256];
	char path[4096];
	
	
	const char * allowed[] = 
		{
			"Patient",
		};
	
	parse_config_file(PW_FILE, &users);

	printf("\n");
	for(i=0; i<users.count; i++)
	{
		char * usertype;
		sprintf(path, "%s/%s/profile.txt", DATABASE_PATH, users.entries[i].key);
		parse_config_file(path, &profile);

		usertype = get_config_value(&profile, "USERTYPE");
		if(usertype == NULL)
		{
			printf("error: %s doesn't have USERTYPE\n", users.entries[i].key);
			continue;
		}
		
		if(strcmp(usertype, "Patient") == 0)
		{
			sprintf(path, "%s/%s/complaints", DATABASE_PATH, users.entries[i].key);
			files = list_files_indir(path, &complaints_n);
			printf("%s has %d complaints\n", users.entries[i].key, complaints_n);

			for(ii=0; ii<complaints_n; ii++)
			{
				printf("> %s\n", files[ii]);
				free(files[ii]);
			}
			free(files);
			printf("\n");
		}
	}

	printf("\nPress enter to continue\n");
	getchar();

	user_select_menu(allowed, 1, patient);
	if(strcmp(patient, "") == 0)
		return make_string("EOF");

	sprintf(path, "%s/%s/complaints/", DATABASE_PATH, patient);
	files = list_files_indir(path, &complaints_n);

	if
	( simple_select(COLOR_YELLOW, COLOR_BLACK, complaints_n, (const char **)files, filename, "Choose a complaint")
		== -1 )
	{
		for(i=0; i<complaints_n; i++)
			free(files[i]);
		free(files);
		return make_string("EOF");	
	}
	for(i=0; i<complaints_n; i++)
		free(files[i]);
	free(files);

	sprintf(path, "%s/%s/complaints/%s", DATABASE_PATH, patient, filename);
	text_editor(path, 1);
	printf("Delete file %s? [y/n]: ", path);
	wipe_stdin();
	if(getchar() == 'y')
	{
		remove(path);
		printf("Deleted %s\n", path);
	}
	
	return make_int(0);
}
