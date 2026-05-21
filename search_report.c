#include <stdio.h>
#include "bookings.h"
#include "logins.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "commands.h"

/* file handling */
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

/* TUI */
#include "tui.h"
#ifndef _WIN32
	#include <ncurses.h>
#else
	#include <ncursesw/curses.h>
	#include <windows.h>
	#include <wincrypt.h>
#endif

#define MAX_NAME 60
#define MAX_ID 20
#define DATE_LEN 11
#define MAX_DIAGNOSIS 100
#define MAX_SUMMARY 300
#define MAX_BLOOD_TYPE 5
#define MAX_HEIGHT 10
#define MAX_BLOOD_TYPES 8

static void read_line(char *buf, int size){
    if(fgets(buf, size, stdin))
            buf[strcspn(buf, "\n")] = 0;
}

extern int patient_conflict(const char *patient_name); /*check if patient folder already exists*/

int search_diagnostic_report(void){
    char patient_name[MAX_NAME];
    char filepath[MAX_NAME + 20];
    char line[500];

    printf("SEARCH DIAGNOSTIC REPORT\n");
    printf("Enter patient name: ");
    read_line(patient_name, MAX_NAME);
    if (patient_name[0] == '\0') {
        printf("Patient name cannot be empty.\n");
        return 0;
    }

    if (!patient_conflict(patient_name)) {
        printf("Patient record not found.\n");
        return 0;
    }

#ifdef _WIN32
    snprintf(filepath, sizeof(filepath), "%s\\profile.txt", patient_name);
#else    
    snprintf(filepath, sizeof(filepath), "%s/profile.txt", patient_name);
#endif

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Error opening patient profile");
        return 0;
    }

    printf("Diagnostic Report for %s:\n", patient_name);

    int entry = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Patient:", 8) == 0) {
            if (entry > 0)
                printf("\n");
            entry++;
            printf("[Entry %d]\n", entry);
        }   
            printf("%s", line);
    }

    if (entry == 0) {
        printf("No diagnostic reports found for this patient.\n");
    }

    printf("--- End of Report ---\n");
    fclose(fp);
    return 1;
}