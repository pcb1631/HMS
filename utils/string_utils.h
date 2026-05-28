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
void trim_trailing(char *str);
void string_trim(char * s);
#endif
