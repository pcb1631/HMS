#include <stdio.h>
#include <string.h>
#include <time.h>
#include "complaints.h"
#include "../utils/utils.h"
#include "../utils/string_utils.h"
#include "../utils/globals.h"
#include "../utils/tui.h"

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


/*
void viewComplaints(void) {
    FILE *fp = fopen("complaints.txt", "r");
    Complaint temp;

    if (fp == NULL) {
        printf("Error on opening file.\n");
        return;
    }

    printf("\nPATIENT COMPLAINT REPORT\n");
    while (fscanf(fp, "%s %s %s %[^\n]", temp.patientID, temp,patientName, 
                  temp.complaintDate, temp.description) == 4) {
        printf("ID: %s | Name: %s | Date: %s\n", temp.patientID, temp,patientName, temp.complaintDate);
        printf("Complaint: %s\n", temp.description);

    }
    fclose(fp);
}

void searchComplaint(const char *searchID) {
    FILE *fp = fopen("complaints.txt", "r");
    Complaint temp;
    int found = 0;

    if (fp == NULL) return;

    while (fscanf(fp, "%s %s %s %[^\n]", temp.patientID, temp,patientName, 
                  temp.complaintDate, temp.description) == 4) {
        if(strcmp(temp.patientID, searchID) == 0) {
            printf("\nMatch Found:\nID: %s\nDate: %s\nIssue: %s\n",
                    temp.patientID, temp.complaintDate, temp.description);
            found = 1;
        }
    }
    if (!found) printf("No complaints found for ID: %s\n", searchID);
    fclose(fp);
}
 */