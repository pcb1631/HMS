#include "commands.h"
#include "bookings.h"
#include "../utils/tui.h"
#include "../utils/globals.h"

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SLOTS 100

/* use realloc for this, maybe */
void parse_slots(const char * doctor, Slot * slots, int * slots_n)
{
    int i = 0;
    char filepath[256];
    FILE *sl;
    sprintf(filepath, "./users/%s/bookings.csv.txt", doctor);
    sl = fopen(filepath, "r");

    if (sl == NULL)
    {
        printf("Error opening file for reading! %s\n", filepath);
    }

    while (fscanf(sl, "%ld,%ld,%49[^,],%49s", &slots[i].start, &slots[i].end, slots[i].venue, slots[i].patient) == 4)
    {
        i++;
    }
    *slots_n = i;
    fclose(sl);
}

void write_to_file_booking(const char * doctor, Slot * slots, int slots_n)
{
    FILE * file;
    char path[256];
    int i;

    if(strcmp(doctor, "") == 0)
    {
        fprintf(stderr, "write_to_file_booking doctor is empty\n");
        exit(1);
    }
    sprintf(path, "./users/%s/bookings.csv.txt", doctor);

    file = fopen(path, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file! %s\n", path);
        return;
    }


    for(i=0; i<slots_n; i++)
    {
        fprintf(file, "%ld,%ld,%s,%s\n", slots[i].start, slots[i].end, slots[i].venue, slots[i].patient);
    }

    fclose(file);

    printf("Successfully wrote to file %s\n", path);
}

Value doctor_booking_frontend(void)
{
    char username[64];
    int select_d;
    char select_c[64];
    const char * options[] =
    {
        "Add slot",
        "View slots",
        "Edit slots",
        "Generate slots for this week"
    };
    strcpy(username, current_username);


    clear();
    select_d = simple_select(COLOR_BLUE, COLOR_BLACK, sizeof(options)/sizeof(char *), options, select_c, "Doctor booking options");
    if (select_d == -1)
    	return make_string("EOF/EOT");

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
    char patient[64];
    sprintf(filepath, "./users/%s/bookings.csv.txt", username);


    sl = fopen(filepath, "a");
    if (sl == NULL)
    {
        fprintf(stderr, "Error opening file! %s\n", filepath);
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

    printf("Enter username of patient (or NULL to make empty slot)\n");
    fgets(patient, 64, stdin);
    trim_trailing(patient);
    if ( strcmp(patient, "") == 0)
        strcpy(patient, "NULL");

    fprintf(sl, "%ld,%ld,%s,%s\n", start, end, venue, patient);

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

Value nurse_view_booking(void)
{
    const char * allowed_utype[] =
    {
        "Doctor"
    };
    char selected_doctor[64] = "";
    int select_slot;
    Slot slots[128];
    int slots_n;

    char username[64];
    strcpy(username, current_username);

    user_select_menu(allowed_utype, 1, selected_doctor);

    if(strcmp(selected_doctor, "") == 0)
    {
   		printf("cancelled doctor select\n");
    	return make_string("EOF/EOT");
    }



    parse_slots(selected_doctor, slots, &slots_n);
    select_slot = booking_slot_view(slots, slots_n, selected_doctor, 0);

    if(select_slot == -1)
        return make_string("Booking slots empty");

    return make_string("OK");
}

Value patient_booking_frontend(void)
{
    const char * allowed_utype[] =
    {
        "Doctor"
    };
    char selected_doctor[64] = "";
    int select_slot;
    int select_option;
    char select_s[32]= "";
    const char * null_options[] =
    {
        "Book this slot",
        "Exit"
    };
    const char * booked_options[] =
    {
        "Cancel booking",
        "Exit"
    };

    Slot slots[128];
    int slots_n;

    char username[64];
    strcpy(username, current_username);

    user_select_menu(allowed_utype, 1, selected_doctor);

    if(strcmp(selected_doctor, "") == 0)
    	return make_string("EOF/EOT");
    
    parse_slots(selected_doctor, slots, &slots_n);
    select_slot = booking_slot_view(slots, slots_n, selected_doctor, 1);

    if(select_slot == -1)
        return make_string("Booking slots empty");

    erase();
    if(strcmp(slots[select_slot].patient, username) == 0)
    {
        select_option = simple_select(COLOR_CYAN, COLOR_BLACK, 2, booked_options, select_s, "Slot booked by you");
        if(select_option == 0)
            strcpy(slots[select_slot].patient, "NULL");

        else if (select_option == -1)
        	return make_string("EOF");
    }
    else if(strcmp(slots[select_slot].patient, "NULL") == 0)
    {
        select_option = simple_select(COLOR_CYAN, COLOR_BLACK, 2, null_options, select_s, "Vacant slot");
        if(select_option == 0)
            strcpy(slots[select_slot].patient, username);

        else if (select_option == -1)
        	return make_string("EOF");
    }

    write_to_file_booking(selected_doctor, slots, slots_n);

    return make_string("OK");
}
/*
int compare_slots(const void *a, const void *b) {
    Slot *slot_a = (Slot *)a;
    Slot *slot_b = (Slot *)b;
    return strcmp(slot_a->start, slot_b->start);

    return 0;
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
