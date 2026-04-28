#include "commands.h"
#include "utils.h"
#include "logins.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#define MAX_SLOTS 100


typedef struct {
    int index;        // slot number
    char date[20];    // string for date (YYYY-MM-DD)
    char start[10];   // start time (HH:MM)
    char end[10];     // end time (HH:MM)
    char status[10];  // "free" or "booked"
} Slot;


int compare_slots(const void *a, const void *b) {
    Slot *slot_a = (Slot *)a;
    Slot *slot_b = (Slot *)b;
    return strcmp(slot_a->start, slot_b->start);

    return 0;
}


void sort_slots(){
    FILE *z = fopen("slots.txt", "r");
    if (z == NULL) {
        printf("Error opening file for reading!\n");
        return;
    }

    Slot slots[MAX_SLOTS];
    int count = 0;

    // Read slots from file
    while (fscanf(z, "%d,%19[^,],%9[^,],%9[^,],%9s",
                  &slots[count].index,
                  slots[count].date,
                  slots[count].start,
                  slots[count].end,
                  slots[count].status) == 5) {
        count++;
    }
    fclose(z);

    // Sort slots by start time
    qsort(slots, count, sizeof(Slot), compare_slots);

    // Rewrite file with new indices
    z = fopen("slots.txt", "w");
    if (z == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(z, "%d,%s,%s,%s,%s\n",
                i + 1, slots[i].date, slots[i].start, slots[i].end, slots[i].status);
    }

    fclose(z);
    printf("Slots sorted and re-indexed!\n");

    return 0;
}


void patient_booking(){

    int user_input;
    do{
        printf("Select an option: "
            "\n1. Book slot"
            "\n2. View booking"
            "\n3. Reschedule booking"
            "\n4. Cancel booking");
        scanf("%d", &user_input);


        if (user_input == 1){
            patient_booking_selection();
        } else if (user_input == 2){
            patient_view_booking();
        } else if (user_input == 3){
        patient_reschedule();
        } else if (user_input == 4){
           patient_cancel();
        } else{
            printf("Invalid option");
        }
     } while(user_input <= 4 && user_input >= 1);

     return 0;
}


/***  idk what to do for this yet

float patient_booking_selection(int doctorID, char*slotTime, char *status){

    FILE *fp = fopen("slots.txt", "r");
    FILE *temp = fopen("temp.txt","w");
    char line[100];

    
    
}

*/





void generate_slots() {
    FILE *sl = fopen("slots.txt", "w");
    if (sl == NULL) {
        printf("Error opening file!\n");
        return;
    }

    time_t now = time(NULL); // Gets the current time in seconds form (epoch)
    struct tm slot_time = *localtime(&now); // Converts epoch into readable date time format

    int index = 1; // create index for slots

    // Loops through 7 days
    for (int day = 0; day < 7; day++) {
        struct tm current_day = slot_time; // Set to the current day
        current_day.tm_mday += day; // counts the days
        mktime(&current_day); // converts time to readable format
        
        // Loop through hours 8 to 17 (8am-5pm)
        for (int hour = 8; hour <= 17; hour++) {
            struct tm start = current_day;
            start.tm_hour = hour;
            start.tm_min = 0;
            start.tm_sec = 0;

            struct tm end = start;
            end.tm_hour = hour + 1; // one hour slot
            mktime(&end); // normalize format

            char dateStr[20], startStr[10], endStr[10];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &start);
            strftime(startStr, sizeof(startStr), "%H:%M", &start);
            strftime(endStr, sizeof(endStr), "%H:%M", &end);


            //Writes the slot details into slots.txt file
            fprintf(sl, "%d,%s,%s,%s,free\n",index, dateStr, startStr, endStr);
            index ++; //incremetal of 1 per slot
        }
    }

    fclose(sl);
    printf("Slots generated for the next 7 days!\n");

    sort_slots();
}


void add_slot() {
    FILE *sl = fopen("slots.txt", "a"); // open file in append mode
    if (sl == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int index;
    char date[20], start[10], end[10];

    printf("Enter slot index number: ");
    scanf("%d", &index);

    printf("Enter date (YYYY-MM-DD): ");
    scanf("%s", date);

    printf("Enter start time (HH:MM): ");
    scanf("%s", start);

    printf("Enter end time (HH:MM): ");
    scanf("%s", end);

    // Write slot into file
    fprintf(sl, "%d,%s,%s,%s,free\n", index, date, start, end);

    fclose(sl);
    printf("Slot added successfully!\n");

    sort_slots();
}


void edit_slot() {
    FILE *sl = fopen("slots.txt", "r");
    if (sl == NULL) {
        printf("Error opening file for reading!\n");
        return;
    }

    Slot slots[MAX_SLOTS];
    int count = 0;

    // Load slots into memory
    while (fscanf(sl, "%d,%19[^,],%9[^,],%9[^,],%9s",
                  &slots[count].index,
                  slots[count].date,
                  slots[count].start,
                  slots[count].end,
                  slots[count].status) == 5) {
        count++;
    }
    fclose(sl);

    // Ask user which slot to edit
    int slot_index;
    printf("Enter the slot index to edit: ");
    scanf("%d", &slot_index);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (slots[i].index == slot_index) {
            found = 1;
            printf("Editing slot %d (%s %s-%s, %s)\n",
                   slots[i].index, slots[i].date,
                   slots[i].start, slots[i].end, slots[i].status);

            printf("Enter new date (YYYY-MM-DD): ");
            scanf("%s", slots[i].date);

            printf("Enter new start time (HH:MM): ");
            scanf("%s", slots[i].start);

            printf("Enter new end time (HH:MM): ");
            scanf("%s", slots[i].end);

            printf("Enter new status (free/booked): ");
            scanf("%s", slots[i].status);
            break;
        }
    }

    if (!found) {
        printf("Slot with index %d not found!\n", slot_index);
        return;
    }

    // Rewrite all slots back to file
    sl = fopen("slots.txt", "w");
    if (sl == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(sl, "%d,%s,%s,%s,%s\n",
                slots[i].index,
                slots[i].date,
                slots[i].start,
                slots[i].end,
                slots[i].status);
    }

    fclose(sl);
    printf("Slot updated successfully!\n");

    sort_slots();
}


void view_slots() {
    FILE *sl = fopen("slots.txt", "r");
    if (sl == NULL) {
        printf("Error opening file for reading!\n");
        return;
    }

    Slot slot;
    printf("\n--- Current Slots ---\n");
    while (fscanf(sl, "%d,%19[^,],%9[^,],%9[^,],%9s",
                  &slot.index,
                  slot.date,
                  slot.start,
                  slot.end,
                  slot.status) == 5) {
        printf("%d | %s | %s-%s | %s\n",
               slot.index, slot.date, slot.start, slot.end, slot.status);
    }

    fclose(sl);
    printf("----------------------\n");
}
