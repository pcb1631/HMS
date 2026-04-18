#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief Utility functions, value types, and command parsing.
 */

typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_VOID
} ValueType;

/**
 * @brief Something like a python variable
 *
 * Can make a python list by defining a Value array
 */
typedef struct
{
    /** To label the datatype*/
    ValueType type;

    /** Actual data */
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

/** Type-erased function pointer. void (*)(void), used like generic_fn x; */
typedef void (*generic_fn)(void);


/**
 * @brief This struct contains information of a function, as our method of calling arbitrary functions are type-erased function pointers.
 */
typedef struct
{
    /** The type-erased function pointer. */
    generic_fn 	ptr;

    char * 		alias;
    char * 		permission;

    /** -1 means any amount */
    int			n_of_args;

    /** enum array */
    ValueType * arg_type;
} command;

/**
 * @brief Macro to define a command with arguments
 */
#define COMMAND(fn, alias, perm, arg_types) \
    { (generic_fn)(fn), (alias), (perm), sizeof(arg_types)/sizeof(arg_types[0]), (arg_types) }

#define COMMAND_NOARGS(fn, alias, perm) \
    { (generic_fn)(fn), (alias), (perm), 0, NULL }

void cmd_parse(char* str);

/**
 * @brief Example usage of "Value" array (1)
 */
Value add(Value * list);
Value meow(Value * list);
Value cat(Value * list);

/**
 * @brief Example usage of "Value" array (2)
 */
void print_list(Value * list, int n);

/**
 * @brief Call arbitrary function
 * @param f Function pointer
 * @param args Arguments to pass to the called function. Must be a pointer to a value array
 * @param n_of_args
 * @param perm String of the current user's usertype
 * @return Returns a Value variable.
 */
Value call(command f, Value * args, int n_of_args, char * perm);
int string_trim(char * s);

#endif
