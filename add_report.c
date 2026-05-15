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

static int valid_date(const char *date){
    int d, m, y;
    if (strlen(date) !=10)
        return 0;
    if (date[2] != '/' || date[5] != '/')
        return 0;
    if (sscanf(date, "%d/%d/%d", &d, &m, &y) != 3)
        return 0;
    if (y < 1900 || y > 2100)
        return 0;
    if (m < 1 || m > 12)
        return 0;
    if (d < 1 || d > 31)
        return 0;
    return 1;
}

static int valid_blood_type(const char *blood_type){
    const char *valid[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-", NULL};
    for(int i = 0; valid[i] != NULL; i++)
        if(strcmp(blood_type, valid[i]) == 0)
            return 1;
    return 0;
}

int patient_conflict(const char *patient_name){
    struct stat info;
    if(stat(patient_name, &info) == 0){
        return 1; // File exists
    } else {
        return 0; // File does not exist
    }
}

void create_patient_file(const char *patient_name){
    FILE *file = fopen(patient_name, "w");
    if(file == NULL){
        perror("Error creating patient file");
        return;
    }
    fclose(file);
}


void create_patient_folder(const char *patient_name){
    if(patient_conflict(patient_name)){
        printf("Patient record already exists.\n");
        return;
    }
    
    #ifdef _WIN32
        if (_mkdir(patient_name) != 0) {
    #else
        if (mkdir(patient_name, 0755) != 0) {
    #endif
        	perror("Error creating patient folder");
            return;
        }

    char filepath[300];

#ifdef _WIN32
    snprintf(filepath, sizeof(filepath), "%s\\payments.csv.txt", patient_name);
    create_patient_file(filepath);

    snprintf(filepath, sizeof(filepath), "%s\\prefs.txt", patient_name);
    create_patient_file(filepath);

    snprintf(filepath, sizeof(filepath), "%s\\profile.txt", patient_name);
    create_patient_file(filepath);
#else
    snprintf(filepath, sizeof(filepath), "%s/payments.csv.txt", patient_name);
    create_patient_file(filepath);

    snprintf(filepath, sizeof(filepath), "%s/prefs.txt", patient_name);
    create_patient_file(filepath);

    snprintf(filepath, sizeof(filepath), "%s/profile.txt", patient_name);
    create_patient_file(filepath);
#endif

}

int add_diagnostic_report(void)
{
    char patient_name[MAX_NAME];
    char blood_type[MAX_BLOOD_TYPE];
    char height[MAX_HEIGHT];
    char date[DATE_LEN];
    char diagnosis[MAX_DIAGNOSIS];

    printf("ADD DIAGNOSTIC INFORMATION\n");

    printf("Enter patient name: ");
    read_line(patient_name, MAX_NAME);
    if (patient_name[0] == '\0') {
        printf("Patient name cannot be empty.\n");
        return 0;
    }

    if (!patient_conflict(patient_name)) {
        create_patient_folder(patient_name);
    }

    do{
        printf(" Blood Type (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
        read_line(blood_type, MAX_BLOOD_TYPE);
        if (!valid_blood_type(blood_type))
            printf("Invalid blood type. Please enter a valid blood type.\n");
    } while (!valid_blood_type(blood_type));

    do{
        printf(" Height (cm): ");
        read_line(height, MAX_HEIGHT);
        float h = 0.0f;
        if (sscanf(height, "%f", &h) != 1 || h <= 0.0f) {
            printf("Invalid height. Please enter a positive number.\n");
            height[0] = '\0'; // Clear height to trigger re-entry
        }
    } while (height[0] == '\0');

    do {
        printf(" Date (DD/MM/YYYY): ");
        read_line(date, DATE_LEN);
        if (!valid_date(date))
            printf("Invalid date format. Please enter a valid date in DD/MM/YYYY format.\n");
    } while (!valid_date(date));

    printf(" Diagnostic Summary\n");
    printf(" (symptoms, findings, treatment plan, etc.)\n");
    printf(" > ");
    read_line(diagnosis, MAX_DIAGNOSIS);

    char filepath[MAX_NAME + 20];
#ifdef _WIN32
    snprintf(filepath, sizeof(filepath), "%s\\profile.txt", patient_name);
#else
    snprintf(filepath, sizeof(filepath), "%s/profile.txt", patient_name);
#endif

    FILE *fp = fopen(filepath, "a");
    if (fp == NULL) {
        perror("Error opening patient profile file");
        return 0;
    }

    fprintf(fp, "Patient: %s\n", patient_name);
    fprintf(fp, "Date: %s\n", date);
    fprintf(fp, "Blood Type: %s\n", blood_type);
    fprintf(fp, "Height: %s cm\n", height);
    fprintf(fp, "Diagnosis:\n%s\n", diagnosis);

    fclose(fp);
    printf("Diagnostic report added successfully.\n");
    return 1;
}