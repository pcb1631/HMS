#include "commands.h"
#include "bookings.h"
#include "tui.h"
#include "globals.h"

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SLOTS 100


Value doctor_booking_frontend(void)
{
    char username[64];
    int select_d;
    char select_c[64];
    char * options[2] =
    {
        "Add slot",
        "View slots"
    };
    strcpy(username, current_username);


    clear();
    select_d = simple_select(COLOR_BLUE, COLOR_BLACK, sizeof(options)/sizeof(char *), options, select_c, "Doctor booking options");

    switch (select_d)
    {
        case 0:
        add_slot(username);
        break;

        case 1:
        view_slots(username);
        break;

        default:
        break;
    }


    return make_int(0);
}

void add_slot(char * username)
{
    /*open file in append mode*/
    time_t start, end;
    char venue[64];
    FILE *sl;
    char filepath[256];
    sprintf(filepath, "./users/%s/bookings.csv.txt", username);


    sl = fopen(filepath, "a");
    if (sl == NULL)
    {
        printf("Error opening file!\n");
        printf("%s\n", filepath);
        return;
    }

    erase();
    time(&start);
    start = time_picker
        ("Appointment start time.", COLOR_BLUE, COLOR_BLACK, start);

    erase();
    end = time_picker("Appointment end time", COLOR_BLUE, COLOR_BLACK, start);

    printf("Enter venue of appointment\n");
    fgets(venue, 64, stdin);
    trim_trailing(venue);

    fprintf(sl, "%ld,%ld,%s,%s\n", start, end, venue, "NULL");

    fclose(sl);
    printf("Slot added successfully!\n");
}

void view_slots(char * username)
{
    char readable_start[80];
    char readable_end[80];
    struct tm * start_tm;
    struct tm * end_tm;
    Slot slot;

    char filepath[256];
    FILE *sl;
    sprintf(filepath, "./users/%s/bookings.csv.txt", username);
    sl = fopen(filepath, "r");

    if (sl == NULL)
    {
        printf("Error opening file for reading!\n");
        return;
    }

    printf("\n--- Current Slots ---\n\n");
    while (fscanf(sl, "%ld,%ld,%49[^,],%49s", &slot.start, &slot.end, slot.venue, slot.patient) == 4)
    {
        start_tm = localtime(&slot.start);
        end_tm =   localtime(&slot.end);
        strftime(readable_start, sizeof(readable_start),"%Y-%m-%d %H:%M:%S", start_tm);
        strftime(readable_end, sizeof(readable_end),"%Y-%m-%d %H:%M:%S", end_tm);
        printf("start: %s, end: %s, venue: %s, patient: %s\n\n", readable_start, readable_end, slot.venue, slot.patient);
    }



    fclose(sl);
    printf("----------------------\n");
}

/*
void patient_booking()
{
    int user_input;
    printf("Select an option: "
        "\n1. Book slot"
        "\n2. View booking"
        "\n3. Reschedule booking"
        "\n4. Cancel booking");
    scanf("%d", &user_input);

    switch (user_input){
        case 1:
            patient_booking();
            break;
        case 2:
            patient_view_booking();
            break;
        case 3:
            patient_reschedule();
            break;
        case 4:
            patient_cancel();
            break;
        default:
            printf("Invalid option");
            break;
    }
}
int compare_slots(const void *a, const void *b) {
    Slot *slot_a = (Slot *)a;
    Slot *slot_b = (Slot *)b;
    return strcmp(slot_a->start, slot_b->start);

    return 0;
}
 *
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
 */




/***  idk what to do for this yet

float patient_booking_selection(int doctorID, char*slotTime, char *status){

    FILE *fp = fopen("slots.txt", "r");
    FILE *temp = fopen("temp.txt","w");
    char line[100];

}

*/




/*
 *
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
 */



/*
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


void delete_slot(char * username){

    Slot slots[MAX_SLOTS];
    int count;
    int i;
    int j;
    int slot_index;
    int found;

    FILE *sl;
    char filepath[256];

    count = 0;
    found = 0;

    sprintf(filepath, "./users/%s/bookings.csv.txt", username);
    sl = fopen(filepath, "r");
    if (sl == NULL){
        printf("Error opening file for reading!\n");
        return;
    }

    while (count < MAX_SLOTS &&
           fscanf(sl, "%ld,%ld,%49[^,],%49s",
                  &slots[count].start,
                  &slots[count].end,
                  slots[count].venue,
                  slots[count].patient) == 4)
    {
        count ++;
    }

    fclose(sl);
    printf("Enter slot number to delete: ");
    scanf("%d", &slot_index);

    for (i = 0; i < count; i ++){
        if (i == slot_index){
            found = 1;
            for (j = i; j < count - 1; j ++){
                slots[j] = slots[j + 1];
            }
            count --;
            break;
        }
    }

    if (!found){
        printf("Slot not found!\n");
        return;
    }

    sl = fopen(filepath, "w");
    if (sl == NULL){
        printf("Error opening file for writing!\n");
        return;
    }

    for (i = 0; i < count; i ++){
        fprintf(sl, "%ld,%ld,%s,%s\n",
                slots[i].start,
                slots[i].end,
                slots[i].venue,
                slots[i].patient);
    }

    fclose(sl);
    printf("Slot deleted successfully!\n");
}

*/
 
/*Patient bookings functions*/


void patient_booking(char * username)
{
    FILE *sl;
    char filepath[256];
    Slot slots[MAX_SLOTS];

    int count;
    int i;
    int slot_index;
    int found;

    char readable_start[80];
    char readable_end[80];

    struct tm * start_tm;
    struct tm * end_tm;

    count = 0;
    found = 0;

    sprintf(filepath, "./users/%s/bookings.csv.txt", username);

    sl = fopen(filepath, "r");

    if (sl == NULL){
        printf("Error opening file for reading!\n");
        return;
    }

    while (count < MAX_SLOTS &&
           fscanf(sl,
                  "%ld,%ld,%49[^,],%49s",
                  &slots[count].start,
                  &slots[count].end,
                  slots[count].venue,
                  slots[count].patient) == 4)
    {
        count ++;
    }

    fclose(sl);
    printf("\n--- Available Slots ---\n\n");

    for (i = 0; i < count; i ++){
        if (strcmp(slots[i].patient, "NULL") == 0){
            start_tm = localtime(&slots[i].start);
            end_tm   = localtime(&slots[i].end);

            strftime(readable_start,sizeof(readable_start),"%Y-%m-%d %H:%M",start_tm);
            strftime(readable_end,sizeof(readable_end),"%Y-%m-%d %H:%M",end_tm);

            printf("Slot %d\n", i);
            printf("Start  : %s\n", readable_start);
            printf("End    : %s\n", readable_end);
            printf("Venue  : %s\n\n", slots[i].venue);
        }
    }

    printf("Enter slot number to book: ");
    scanf("%d", &slot_index);
    getchar();

    if (slot_index < 0 || slot_index >= count){
        printf("Invalid slot number!\n");
        return;
    }

    if (strcmp(slots[slot_index].patient, "NULL") != 0){
        printf("Slot already booked!\n");
        return;
    }

    strcpy(slots[slot_index].patient, current_username);
    sl = fopen(filepath, "w");

    if (sl == NULL){
        printf("Error opening file for writing!\n");
        return;
    }

    for (i = 0; i < count; i ++){
        fprintf(sl,
                "%ld,%ld,%s,%s\n",
                slots[i].start,
                slots[i].end,
                slots[i].venue,
                slots[i].patient);
    }

    fclose(sl);
    printf("Appointment booked successfully!\n");

}


void patient_view_booking(char * username){

    FILE *sl;
    char filepath[256];
    Slot slot;

    char readable_start[80];
    char readable_end[80];

    struct tm * start_tm;
    struct tm * end_tm;

    int found;
    found = 0;

    sprintf(filepath, "./users/%s/bookings.csv.txt", username);
    sl = fopen(filepath, "r");
    if (sl == NULL){
        printf("Error opening file for reading!\n");
        return;
    }

    printf("\n--- Your Bookings ---\n\n");
    while (fscanf(sl,
                  "%ld,%ld,%49[^,],%49s",
                  &slot.start,
                  &slot.end,
                  slot.venue,
                  slot.patient) == 4)
    {
        if (strcmp(slot.patient, current_username) == 0){
            found = 1;

            start_tm = localtime(&slot.start);
            end_tm   = localtime(&slot.end);

            strftime(readable_start,sizeof(readable_start),"%Y-%m-%d %H:%M",start_tm);
            strftime(readable_end,sizeof(readable_end),"%Y-%m-%d %H:%M",end_tm);

            printf("Start  : %s\n", readable_start);
            printf("End    : %s\n", readable_end);
            printf("Venue  : %s\n\n", slot.venue);
        }
    }

    fclose(sl);
    if (!found){
        printf("No bookings found!\n");
    }
    printf("-------------------------\n");

}

void patient_reschedule(char * username){
    FILE *sl;
    char filepath[256];
    Slot slots[MAX_SLOTS];

    int count;
    int i;
    int booking_index;
    int new_slot_index;
    int found;

    char readable_start[80];
    char readable_end[80];

    struct tm * start_tm;
    struct tm * end_tm;

    count = 0;
    found = 0;

    sprintf(filepath, "./users/%s/bookings.csv.txt", username);
    sl = fopen(filepath, "r");
    if (sl == NULL){
        printf("Error opening file for reading!\n");
        return;
    }

    while (count < MAX_SLOTS &&
           fscanf(sl,
                  "%ld,%ld,%49[^,],%49s",
                  &slots[count].start,
                  &slots[count].end,
                  slots[count].venue,
                  slots[count].patient) == 4)
    {
        count ++;
    }

    fclose(sl);
    printf("\n--- Your Current Bookings ---\n\n");
    for (i = 0; i < count; i ++){
        if (strcmp(slots[i].patient,current_username) == 0){
            found = 1;
            start_tm = localtime(&slots[i].start);
            end_tm   = localtime(&slots[i].end);

            strftime(readable_start,sizeof(readable_start),"%Y-%m-%d %H:%M",start_tm);
            strftime(readable_end,sizeof(readable_end),"%Y-%m-%d %H:%M",end_tm);

            printf("Booking %d\n", i);
            printf("Start  : %s\n", readable_start);
            printf("End    : %s\n", readable_end);
            printf("Venue  : %s\n\n", slots[i].venue);
        }
    }

    if (!found){
        printf("No bookings found!\n");
        return;
    }

    printf("Enter booking number to reschedule: ");
    scanf("%d", &booking_index);
    getchar();

    if (booking_index < 0 || booking_index >= count){
        printf("Invalid booking number!\n");
        return;
    }

    if (strcmp(slots[booking_index].patient,current_username) != 0){
        printf("This booking does not belong to you!\n");
        return;
    }

    printf("\n--- Available Slots ---\n\n");
    for (i = 0; i < count; i ++){
        if (strcmp(slots[i].patient, "NULL") == 0){
            start_tm = localtime(&slots[i].start);
            end_tm   = localtime(&slots[i].end);

            strftime(readable_start,sizeof(readable_start),"%Y-%m-%d %H:%M",start_tm);
            strftime(readable_end,sizeof(readable_end),"%Y-%m-%d %H:%M",end_tm);

            printf("Slot %d\n", i);
            printf("Start  : %s\n", readable_start);
            printf("End    : %s\n", readable_end);
            printf("Venue  : %s\n\n", slots[i].venue);
        }
    }

    printf("Enter new slot number: ");
    scanf("%d", &new_slot_index);
    getchar();

    if (new_slot_index < 0 || new_slot_index >= count){
        printf("Invalid slot number!\n");
        return;
    }

    if (strcmp(slots[new_slot_index].patient, "NULL") != 0){
        printf("Slot already booked!\n");
        return;
    }

    strcpy(slots[new_slot_index].patient, current_username);
    strcpy(slots[booking_index].patient, "NULL");
    sl = fopen(filepath, "w");

    if (sl == NULL){
        printf("Error opening file for writing!\n");
        return;
    }

    for (i = 0; i < count; i ++){
        fprintf(sl,
                "%ld,%ld,%s,%s\n",
                slots[i].start,
                slots[i].end,
                slots[i].venue,
                slots[i].patient);
    }

    fclose(sl);
    printf("Booking rescheduled successfully!\n");
}


void patient_cancel(char * username){
    FILE *sl;
    char filepath[256];
    Slot slots[MAX_SLOTS];

    int count;
    int i;
    int booking_index;
    int found;

    char readable_start[80];
    char readable_end[80];

    struct tm * start_tm;
    struct tm * end_tm;

    count = 0;
    found = 0;

    sprintf(filepath, "./users/%s/bookings.csv.txt", username);
    sl = fopen(filepath, "r");
    if (sl == NULL){
        printf("Error opening file for reading!\n");
        return;
    }

    while (count < MAX_SLOTS &&
           fscanf(sl,
                  "%ld,%ld,%49[^,],%49s",
                  &slots[count].start,
                  &slots[count].end,
                  slots[count].venue,
                  slots[count].patient) == 4)
    {
        count ++;
    }

    fclose(sl);
    printf("\n--- Your Current Bookings ---\n\n");
    for (i = 0; i < count; i ++){
        if (strcmp(slots[i].patient,current_username) == 0){
            found = 1;
            start_tm = localtime(&slots[i].start);
            end_tm   = localtime(&slots[i].end);

            strftime(readable_start,sizeof(readable_start),"%Y-%m-%d %H:%M",start_tm);
            strftime(readable_end,sizeof(readable_end),"%Y-%m-%d %H:%M",end_tm);

            printf("Booking %d\n", i);
            printf("Start  : %s\n", readable_start);
            printf("End    : %s\n", readable_end);
            printf("Venue  : %s\n\n", slots[i].venue);
        }
    }

    if (!found){
        printf("No bookings found!\n");
        return;
    }

    printf("Enter booking number to cancel: ");
    scanf("%d", &booking_index);
    getchar();

    if (booking_index < 0 || booking_index >= count){
        printf("Invalid booking number!\n");
        return;
    }

    if (strcmp(slots[booking_index].patient, current_username) != 0){
        printf("This booking does not belong to you!\n");
        return;
    }

    strcpy(slots[booking_index].patient,"NULL");
    sl = fopen(filepath, "w");
    if (sl == NULL){
        printf("Error opening file for writing!\n");
        return;
    }

    for (i = 0; i < count; i ++){
        fprintf(sl,
                "%ld,%ld,%s,%s\n",
                slots[i].start,
                slots[i].end,
                slots[i].venue,
                slots[i].patient);
    }

    fclose(sl);
    printf("Booking cancelled successfully!\n");
}

