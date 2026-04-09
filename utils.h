#ifndef UTILS_H

#define UTILS_H

typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_VOID
} ValueType;

typedef struct
{
    ValueType type;

    union {
        unsigned int	as_bool : 1;
        float			as_float;
        char *			as_string;
        int         	as_int;
    } data;

} Value;

Value make_int(int x);
Value make_float(float x);
Value make_string(char*s);


typedef void (*generic_fn)(void);

typedef struct
{
    generic_fn 	ptr;

    char * 		alias;
    char * 		permission;

	/* -1 means any amount */
    int			n_of_args;

    /* enum array */
    ValueType * arg_type;
} command;

/* Hoo boy we're gonna need a lot of documentation */
#define COMMAND(fn, alias, perm, arg_types) \
    { (generic_fn)(fn), (alias), (perm), sizeof(arg_types)/sizeof(arg_types[0]), (arg_types) }

#define COMMAND_NOARGS(fn, alias, perm) \
    { (generic_fn)(fn), (alias), (perm), 0, NULL }

void cmd_parse(char* str);

Value add(Value * list);
Value meow(Value * list);

Value call(command f, Value * args, int n_of_args, char * perm);

void print_list(Value * list, int n);

#endif
