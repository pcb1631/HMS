#ifndef BOOKINGS_H
#define BOOKINGS_H

#include "../utils/utils.h"
#include <time.h>

typedef struct
{
    time_t start;
    time_t end;
    char patient[64];
    char venue[64];
}Slot;


void view_slots(char * username);
Value doctor_booking_frontend(void);
Value patient_booking_frontend(void);
void add_slot(char * username);
void parse_slots(const char * doctor, Slot * slots, int * slots_n);
#endif
