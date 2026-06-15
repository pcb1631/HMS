#ifndef BILLING_H
#define BILLING_H

#include "../utils/globals.h"
#include "../utils/utils.h"

typedef struct {
    char date[80];
    char patient[MAX_USERNAME_LEN];
    float amount;
    char desc[256];
} Payment;
Value perform_payment(Value * list);
Value view_payment_history_patient(void);
#endif
