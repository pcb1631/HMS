#ifndef BILLING_H
#define BILLING_H
#include "defs.h"

void performPayment(const char *patientID);
void viewPaymentHistory(const char *patientID);
#endif
