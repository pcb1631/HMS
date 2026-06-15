#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "billing.h"
#include "../utils/globals.h"
#include "../utils/utils.h"
#include "../utils/tui.h"

/* patient: view my payments */
int parse_payments(Payment ** payments, int * n)
{
	FILE * file;
	char path[4096];
	char buf[4096];
	int i=0;
	sprintf(path, "%s/%s", DATABASE_PATH, "payments.csv.txt");
	file = fopen(path, "r");
	if(file == NULL)
	{
		perror("parse_payments");
		return -1;
	}

	*payments = malloc(sizeof(Payment) * 1);
	if(*payments == NULL)
	{
		perror("parse_payments()");
		return -1;
	}

		
	while(fgets(buf, sizeof(buf), file)  != NULL)
	{
		if(i > 0)
		{
			Payment * new_payments = realloc(*payments, sizeof(Payment) * (i+1));
			if(new_payments == NULL)
			{
				perror("parse_payments: realloc");
				free(*payments);
				return -1;
			}
			*payments = new_payments;
		}

		if(sscanf(buf, "%80[^,],%64[^,],%f,%255[^\n]", 
			(*payments)[i].date, 
			(*payments)[i].patient, 
			&(*payments)[i].amount, 
			(*payments)[i].desc) == 4)
		{
			i++;
		}
	}

	*n = i;
	fclose(file);
	return 0;
}

Value perform_payment(Value * list) 
{
	FILE *fp;
	char path[4096];
	char readable_time[80];
	char patient[MAX_USERNAME_LEN];

	time_t now = time(NULL);
	struct tm * ts = localtime(&now);
	strftime(readable_time, sizeof(readable_time), "%Y/%m/%d_%H:%M:%S", ts);

	sprintf(path, "%s/%s", DATABASE_PATH, "payments.csv.txt");
	
	fp = fopen(path, "a");
	if(fp == NULL)
	{
		perror("perform_payment()");
		printf("tried path %s\n", path);
		return make_string("file error");
	}

	strcpy(patient, current_username);
	/*date, patient, amt, desc */
	fprintf(fp, "%s,%s,%.2f,%s\n", readable_time, patient, list[0].data.as_float, list[1].data.as_string);
	fclose(fp);
	return make_string("OK");
}

Value view_payment_history_patient(void)
{
	Payment * billz = NULL;
	int n, i;
	char buf[4096];
	char patient[MAX_USERNAME_LEN];
	char ** options;
	int options_n=0;
	strcpy(patient, current_username);
	parse_payments(&billz, &n);

	options = malloc(sizeof(char *) * n);
	for(i=0; i<n; i++)
	{
		if(strcmp(billz[i].patient, patient) != 0)
			continue;
		options[i] = malloc(sizeof(char) * 4096);
		sprintf(options[i], "%.2f %s %s", billz[i].amount, billz[i].date, billz[i].desc);
		options_n++;
	}


	simple_select(COLOR_YELLOW, COLOR_BLACK, options_n, (const char **)options, buf, "yo billz");

	
	free(billz);
	for(i=0; i<options_n; i++)
		free(options[i]);
	free(options);

	
	return make_int(0);
}

