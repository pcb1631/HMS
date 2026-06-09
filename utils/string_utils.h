#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/**
 * @brief removes one character and shifts the characters after to the left
 * @param str
 * @param pos index of the character to remove
 */
void string_pop(char * str, int pos);

/**
 * @brief inserts a character in a string
 * @param str
 * @param ins Char to insert
 * @param pos index of where the char will go
 * @param n Lenght of string before insert
 */
void string_insert(char * str, char ins, int pos);

/*
 * @brief Parses a file, puts each line in a string pointer array **BUTTTT IT DOESNT FREE THE POINTER FOR YOU**
 * @param path Path of file
 * @param lines Size of string pointer array
 * @return -1 on error, the string pointer array on success
 *
 */
char** file_to_strings(const char * path, int * lines_out);

/**
 * @brief removes whitespace at end of string
 */
void trim_trailing(char *str);

/**
 * @brief removes whitespace at start and end of string
 */
void string_trim(char * s);

int isempty(char * s);
/**
 * @brief Pass the pointer to the char pointer to realloc 
 */
void realloc_string(char ** s, int n);

typedef struct LineNode LineNode;
struct LineNode
{
	char * str;
	int str_n;
	LineNode * prev;
	LineNode * next;
};

/**
 * @brief Allocs and initializes a LineNode
 */
LineNode* new_LineNode(void);

/**
 * @brief Frees an entire Line LL
 */
void free_LineLL(LineNode * Head);
LineNode* file_to_LL_lines(const char * path, int * lines_out);

/**
 * @brief Frees node and stitches back prev and next 
 */
void delete_node_LineLL(LineNode * Node);
void dump_LineLL_to_file(LineNode * Head, const char * path);

int count_digits(int n);

int levenshtein_distance(const char* s, const char* t, int size_of_s, int size_of_t);
#endif
