#include <stdio.h>
#include <string.h>
#include "billing.h"

void performPayment(const char *patientID) {
    FILE *fp = fopen("payments.txt", "a");
    Payment p;

    if (fp == NULL) return;

    strcpy(p.patientID, patientID);
    printf("Enter Payment ID: ");
    scanf("%s", p.paymentID);
    printf("Enter Date (DD/MM/YYYY): ");
    scanf("%s", p.date);
    printf("Enter Amount: ");
    scanf("%f", &p.amount);
    strcpy(p.status, "Paid");
    
    fprintf(fp, "%s %s %s %.2f %s\n", p.patientID, p.paymentID, p.date, p.amount, p.status);
    fclose(fp);
    printf("Payment processed successfully.\n");
}

void viewPaymentHistory(const char *patientID) {
    FILE *fp = fopen("payments.txt", "r");
    Payment temp;
    int found = 0;

    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    printf("\nYOUR PAYMENT HISTORY\n");
    while (fscanf(fp, "%s %s %s %f %s", temp.patientID, temp.paymentID,
                    temp.date, &temp.amount, temp.status) == 5) {
            
        if (strcmp(temp.patientID, patientID) == 0) {
            printf("Date: %s | ID: %s | Amount: RM%.2f | Status: %s\n", temp.date, temp.paymentID, temp.amount, temp.status);
            found = 1;
        }
    }
    if (!found) printf("No past billing activities found.\n");
    fclose(fp);
}
