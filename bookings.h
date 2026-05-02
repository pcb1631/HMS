#ifndef BOOKINGS_H
#define BOOKINGS_H

#include "utils.h"
#include <time.h>

typedef struct
{
    time_t start;
    time_t end;
    char patient[64];
    char venue[64];
}Slot;

Value doctor_booking_frontend(Value * list);
void add_slot(char * username);
#endif
