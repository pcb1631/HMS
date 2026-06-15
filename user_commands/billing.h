#ifndef BILLING_H
#define BILLING_H

#include "../utils/globals.h"
#include "../utils/utils.h"
#include <time.h>

typedef struct {
	time_t t;
    char patient[MAX_USERNAME_LEN];
    float amount;
    char desc[256];
} Payment;
Value perform_payment(Value * list);
Value view_payment_history_patient(void);
Value calc_sales_t_range(void);
Value view_payment_history_director(void);
#endif
