#ifdef _WIN32
	#include <ncursesw/ncurses.h>
#else
	#include <ncurses.h>
#endif

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
	unsigned long time_ul;
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

		if(sscanf(buf, "%lu,%64[^,],%f,%255[^\n]", 
			&time_ul,
			(*payments)[i].patient, 
			&(*payments)[i].amount, 
			(*payments)[i].desc) == 4)
		{
			(*payments)[i].t = (time_t)time_ul;
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
	char patient[MAX_USERNAME_LEN];

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
	fprintf(fp, "%lu,%s,%.2f,%s\n", (unsigned long)time(NULL), patient, list[0].data.as_float, list[1].data.as_string);
	fclose(fp);
	return make_string("OK");
}

Value view_payment_history_patient(void)
{
	Payment * billz = NULL;
	int n, i;
	char buf[4096];
	char patient[MAX_USERNAME_LEN];

	char readable_time[80];
	struct tm * ts;
	
	char ** options;
	int options_n=0;
	strcpy(patient, current_username);
	parse_payments(&billz, &n);

	options = malloc(sizeof(char *) * n);
	for(i=0; i<n; i++)
	{
		if(strcmp(billz[i].patient, patient) != 0)
			continue;

		options[options_n] = malloc(sizeof(char) * 4096);
		ts = localtime(&billz[i].t);
		strftime(readable_time, sizeof(readable_time), "%d/%m/%Y %H:%M:%S", ts);
		sprintf(options[options_n], "%s RM%.2f %s", readable_time, billz[i].amount, billz[i].desc);
		options_n++;
	}


	simple_select(COLOR_YELLOW, COLOR_BLACK, options_n, (const char **)options, buf, "yo billz");

	
	free(billz);
	for(i=0; i<options_n; i++)
		free(options[i]);
	free(options);

	
	return make_int(0);
}


Value view_payment_history_director(void)
{
	Payment * billz = NULL;
	int n, i;
	char buf[4096];

	char readable_time[80];
	struct tm * ts;
	
	char ** options;
	parse_payments(&billz, &n);

	options = malloc(sizeof(char *) * n);
	for(i=0; i<n; i++)
	{
		options[i] = malloc(sizeof(char) * 4096);
		ts = localtime(&billz[i].t);
		strftime(readable_time, sizeof(readable_time), "%d/%m/%Y %H:%M:%S", ts);
		sprintf(options[i], "%s RM%.2f %s", readable_time, billz[i].amount, billz[i].desc);
	}


	simple_select(COLOR_YELLOW, COLOR_BLACK, n, (const char **)options, buf, "yo billz");

	
	free(billz);
	for(i=0; i<n; i++)
		free(options[i]);
	free(options);

	
	return make_int(0);
}

Value calc_sales_t_range(void)
{
	time_t start, end;
	char readable_start[80], readable_end[80]; 
	struct tm * ts;


	Payment * payments;
	int i, n;
	float total=0;
	parse_payments(&payments, &n);
	start = time_picker("Start time of sales", COLOR_YELLOW, COLOR_BLACK, time(NULL));
	end = time_picker("End time of sales", COLOR_YELLOW, COLOR_BLACK, time(NULL));

	if(start == -1 || end == -1)
		return make_string("cancelled");

	ts = localtime(&start);
	strftime(readable_start, sizeof(readable_start), "%d/%m/%Y %H:%M:%S", ts);
	ts = localtime(&end);
	strftime(readable_end, sizeof(readable_end), "%d/%m/%Y %H:%M:%S", ts);


	
	for(i=0; i<n; i++)
	{
		if(payments[i].t>=start && payments[i].t <=end)
			total += payments[i].amount;
	}

	printf("The total earnings between *%s* and *%s* is RM%.2f\n", readable_start, readable_end, total);
	return make_string("ok");
}