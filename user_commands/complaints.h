#ifndef COMPLAINTS_H
#define COMPLAINTS_H
#include "../utils/utils.h"

Value view_complaints(void);
Value create_complaint(void);

#define MAX_NAME 50
#define MAX_ID 15
#define MAX_DATE 12
#define MAX_TEXT 200
#define MAX_TYPE 20

typedef struct {
    char patientID[MAX_ID];
    char patientName[MAX_NAME];
    char complaintDate[MAX_DATE];
    char description[MAX_TEXT];
} Complaint;

#endif
