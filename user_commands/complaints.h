/**
 * @file complaints.h
 * @brief User commands for complaint management
 */

#ifndef COMPLAINTS_H
#define COMPLAINTS_H
#include "../utils/utils.h"

Value view_complaints(void);
Value create_complaint(void);
Value view_complaints_self(void);

#endif
