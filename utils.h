#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief Utility functions, value types, and command parsing.
 */
#ifndef _WIN32
    #include <ncurses.h>
#else
    #include <ncursesw/curses.h>
#endif

#define PW_FILE "./users/creds.txt"

#define OR ||
#define AND &&

#include <stdlib.h>
#include <signal.h>
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
        char			as_string[128];
        int         	as_int;
    } data;

} Value;

Value make_int(int x);
Value make_float(float x);
Value make_string(char*s);

/** Type-erased function pointer. void (*)(void), used like generic_fn x; */
typedef void (*generic_fn)(void);


/**
 * @brief This struct contains information of a function, as we use manual vtable dispatch for menu
 *
 * Not calling the function pointer with correct return and arg types WILL result in undefined behaviour
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

    /** Description to be displayed in command selector */
    char *      desc;
} command;

typedef struct command_node command_node;
struct command_node
{
    command f;
    command_node* next;
};
command_node* new_command_node(command f);
void free_cmd_list(command_node* head);


/**
 * @brief Macro to define a command with arguments
 */
#define COMMAND(fn, alias, perm, arg_types, desc) \
    { (generic_fn)(fn), (alias), (perm), sizeof(arg_types)/sizeof(arg_types[0]), (arg_types), desc}

#define COMMAND_NOARGS(fn, alias, perm, desc) \
    { (generic_fn)(fn), (alias), (perm), 0, NULL, desc}

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
void trim_trailing(char *str) ;

#define MAX_CONFIG_ENTRIES 64
#define MAX_CONFIG_KEY_LEN 64
#define MAX_CONFIG_VAL_LEN 256

/**
 * @brief Key-value pair for configuration entries
 */
typedef struct {
    char key[MAX_CONFIG_KEY_LEN];
    char value[MAX_CONFIG_VAL_LEN];
} ConfigEntry;

/**
 * @brief Configuration structure holding multiple entries
 * Used for any file with key=value format
 * You can access a specific entry by doing x.entries[i].key or .value
 */
typedef struct {
    ConfigEntry entries[MAX_CONFIG_ENTRIES];
    int count;
} Config;

/**
 * @brief Parses a configuration file into a Config struct
 * @param filepath Path to the configuration file
 * @param config Pointer to the Config struct to populate
 * @return 0 on success, -1 on failure
 */
int parse_config_file(const char *filepath, Config *config);

/**
 * @brief Gets a value from a Config struct by key
 * @param config Pointer to the Config struct
 * @param key The key to look up
 * @return Pointer to the value string, or NULL if not found
 */
const char *get_config_value(const Config *config, const char *key);

/**
 * @brief Checks if a file or directory exists
 * @param path Path to the file or directory to check
 * @return 1 if exists, 0 if does not exist
 */
int path_exists(const char *path);

#endif

/**
 * @brief Parses file_struct.yaml.txt and generates files/directories for a user
 */
void generate_file_structure(const char *role, const char *username);
void handle_sigint(int sig);

const char * get_utype(const char * username);
