#include "notes.h"
#include "../utils/utils.h"
#include "../utils/tui.h"
#include "../utils/string_utils.h"
#include "../utils/globals.h"
#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

Value create_note(void)
{
	FILE * file;
	char path[4096];
	char filename[256];
	char nurse[MAX_USERNAME_LEN];
	char patient[MAX_USERNAME_LEN];
	const char * allowed[]=
		{
			"Patient"
		};
	
	char readable_time[80];

	time_t now = time(NULL);
	struct tm *ts = localtime(&now);
	
	strcpy(nurse, current_username);
	strftime(readable_time, sizeof(readable_time), "%Y-%m-%d_%H_%M_%S", ts);

	user_select_menu(allowed, 1, patient);
	if(strcmp(patient, "") == 0)
		return make_string("Cancelled");
	
	printf("Enter file name, or leave empty to use current time\n");
	fgets(filename, 256, stdin);
	string_trim(filename);

	if(isempty(filename))
		sprintf(path, "%s/%s/notes/%s.txt", DATABASE_PATH, patient, readable_time);
	else
		sprintf(path, "%s/%s/notes/%s.txt", DATABASE_PATH, patient, filename);
		
	if(path_exists(path))
	{
		text_editor(path, 0);
	}
	else 
	{
		file = fopen(path, "w");
		if(file == NULL)
		{
			perror("create_note()");
			return make_int(-1);
		}
		fprintf(file, "%s %s", nurse, readable_time);
		fclose(file);

		text_editor(path, 0);
	}
	
	return make_int(0);
}

Value view_notes(void)
{
	Config users;
	Config profile;
	char patient[MAX_USERNAME_LEN];
	int i, ii, notes_n;
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
		char * usertype, * username;
		username = users.entries[i].key;
		sprintf(path, "%s/%s/profile.txt", DATABASE_PATH, username);
		parse_config_file(path, &profile);

		usertype = get_config_value(&profile, "USERTYPE");
		if(usertype == NULL)
		{
			printf("error: %s doesn't have USERTYPE\n", username);
			continue;
		}
		
		if(strcmp(usertype, "Patient") == 0)
		{
			sprintf(path, "%s/%s/notes", DATABASE_PATH, username);
			files = list_files_indir(path, &notes_n);

			if(notes_n != 0)
				printf("%s has %d notes\n", username, notes_n);
			else
			{
				for(ii=0; ii<notes_n; ii++)
				{
					printf("> %s\n", files[ii]);
					free(files[ii]);
				}
				free(files);
				continue;
			}

			for(ii=0; ii<notes_n; ii++)
			{
				printf("> %s\n", files[ii]);
				free(files[ii]);
			}
			free(files);

			if(notes_n !=0)
				printf("\n");
		}
	}

	printf("\nPress enter to continue\n");
	getchar();

	user_select_menu(allowed, 1, patient);
	if(strcmp(patient, "") == 0)
		return make_string("EOF");

	sprintf(path, "%s/%s/notes/", DATABASE_PATH, patient);
	files = list_files_indir(path, &notes_n);

	if
	( simple_select(COLOR_YELLOW, COLOR_BLACK, notes_n, (const char **)files, filename, "Choose a complaint")
		== -1 )
	{
		for(i=0; i<notes_n; i++)
			free(files[i]);
		free(files);
		return make_string("EOF");	
	}
	for(i=0; i<notes_n; i++)
		free(files[i]);
	free(files);

	sprintf(path, "%s/%s/notes/%s", DATABASE_PATH, patient, filename);
	text_editor(path, 0);
	return make_int(0);
}