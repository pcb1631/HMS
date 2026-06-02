#ifndef BOOKINGS_H
#define BOOKINGS_H

#include "../utils/utils.h"
#include <time.h>

/**
 * @brief appointment slot
 * We will assume that time_t is always non-negative, so we cast it to and fro unsigned long when reading and printing \n
 * This means if the project is compiled or ran on 32bit windows, it will overflow on year 2106. 
 */
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
Value nurse_view_booking(void);



void add_slot(char * username);
void parse_slots(const char * doctor, Slot * slots, int * slots_n);
#endif
