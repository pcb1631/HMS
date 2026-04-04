#ifndef UTILS_H

#define UTILS_H

#include <stdio.h>
typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL
} ValueType;

typedef struct
{
    ValueType type;

    union {
        int         as_int;
        float       as_float;
        char *		as_string;
        int         as_bool;
    } data;

} Value;

Value make_int(int x);
Value make_float(float x);
Value make_string(char*s);




typedef void (*generic_fn)(void);

typedef enum
{
    FN_INT_BINARY,   /* int (*)(int, int) */
    FN_VOID_INT,     /* void (*)(int)     */
    FN_INT_VOID      /* int (*)(void)     */
} FnTag;

typedef struct
{
    generic_fn ptr;
    FnTag      tag;
} command;


void login(char* username, char* password);
void reg(void); /* register is a reserved keyword */
void cmd_parse(char* str);

int add(Value * list);
void print_list(Value * list, int n);

#endif
