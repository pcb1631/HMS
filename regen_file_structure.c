/*
 * @file regen_file_structure.c
 * This file should be used to make one binary to regenerate user file sturcture, in case you change file_struct.yaml.txt
 */


#include <stdio.h>
#include <string.h>
#include "utils/utils.h"

signed main(void)
{
	Config users;
	int n;
	int i=0;
	char path[64];


	parse_config_file("./users/creds.txt", &users);
	n = users.count;

	for(i=0; i<n; i++)
	{
		char current_username[64];
		char current_utype[64];
		Config cfg;
		const char *val;
		
		printf("%s\n", users.entries[i].key);

		strcpy(current_username, users.entries[i].key);
		sprintf(path, "./users/%s/profile.txt", current_username);
		if (parse_config_file(path, &cfg) == 0)
		{
			val = get_config_value(&cfg, "USERTYPE");
			if (val != NULL)
			{
				strncpy(current_utype, val, sizeof(current_utype) - 1);
				current_utype[sizeof(current_utype) - 1] = '\0';

				generate_file_structure(current_utype, current_username);
			}else
			{
				fprintf(stderr, "Usertype is empty: %s\n", current_username);
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Profile is empty / failed to open file (%s)\n", current_username);
			return 1;
		}
		printf("\n");
		
	}
	return 0;
}
