#include <stdio.h>
#include <string.h>
#include "complaints.h"

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