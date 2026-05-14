#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Report {
    int id;
    char name[50];
    char report[200];
};

struct Complaint {
    int id;
    char patientName[50];
    char complaint[200];
};

void viewDoctorReports() {
    FILE *fp = fopen("doctor.txt", "r");

    struct Report r;

    if (fp == NULL) {
        printf("Doctor report file not found.\n");
        return;
    }

    printf("\n===== DOCTOR REPORTS =====\n");

    while (fread(&r, sizeof(r), 1, fp)) {
        printf("ID: %d\n", r.id);
        printf("Doctor Name: %s", r.name);
        printf("Report: %s\n", r.report);
    }

    fclose(fp);
}

void searchDoctorReport() {
    FILE *fp = fopen("doctor.txt", "r");

    struct Report r;
    int searchID;
    int found = 0;

    if (fp == NULL) {
        printf("Doctor report file not found.\n");
        return;
    }

    printf("Enter Doctor ID to search: ");
    scanf("%d", &searchID);

    while (fread(&r, sizeof(r), 1, fp)) {
        if (r.id == searchID) {

            printf("\nDoctor Report Found\n");
            printf("ID: %d\n", r.id);
            printf("Doctor Name: %s", r.name);
            printf("Report: %s\n", r.report);

            found = 1;
        }
    }

    if (!found) {
        printf("Doctor report not found.\n");
    }

    fclose(fp);
}

void viewNurseReports() {
    FILE *fp = fopen("nurse.txt", "r");

    struct Report r;

    if (fp == NULL) {
        printf("Nurse report file not found.\n");
        return;
    }

    printf("\n===== NURSE REPORTS =====\n");

    while (fread(&r, sizeof(r), 1, fp)) {
        printf("ID: %d\n", r.id);
        printf("Nurse Name: %s", r.name);
        printf("Report: %s\n", r.report);
    }

    fclose(fp);
}

void viewComplaints() {
    FILE *fp = fopen("complaint.txt", "r");

    struct Complaint c;

    if (fp == NULL) {
        printf("Complaint file not found.\n");
        return;
    }

    printf("\n===== PATIENT COMPLAINTS =====\n");

    while (fread(&c, sizeof(c), 1, fp)) {
        printf("ID: %d\n", c.id);
        printf("Patient Name: %s", c.patientName);
        printf("Complaint: %s\n", c.complaint);
    }

    fclose(fp);
}

int main() {

    int choice;

    do {

        printf("\n===== ADMINISTRATOR MENU =====\n");
        printf("1. View Doctor Reports\n");
        printf("2. Search Doctor Report\n");
        printf("3. View Nurse Reports\n");
        printf("4. View Patient Complaints\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {

            case 1:
                viewDoctorReports();
                break;

            case 2:
                searchDoctorReport();
                break;

            case 3:
                viewNurseReports();
                break;

            case 4:
                viewComplaints();
                break;

            case 5:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while(choice != 5);

    return 0;
}