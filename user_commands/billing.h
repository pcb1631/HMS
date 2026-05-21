#ifndef BILLING_H
#define BILLING_H

#define MAX_NAME 50
#define MAX_ID 15
#define MAX_DATE 12
#define MAX_TEXT 200
#define MAX_TYPE 20
typedef struct {
    char patientID[MAX_ID];
    char paymentID[MAX_ID];
    char date[MAX_DATE];
    float amount;
    char status[MAX_TYPE];
} Payment;
void performPayment(const char *patientID);
void viewPaymentHistory(const char *patientID);
#endif
