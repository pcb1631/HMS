#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


Value make_int(int x)          { Value v; v.type = TYPE_INT;    v.data.as_int    = x; return v; }
Value make_float(float x)      { Value v; v.type = TYPE_FLOAT;  v.data.as_float  = x; return v; }
Value make_string(char * s){ Value v; v.type = TYPE_STRING; v.data.as_string = s; return v; }


/* Example usage of "Value" array (1)*/
int add(Value* list)
{
	if (list[0].type != TYPE_INT || list[1].type != TYPE_INT)
	{
		fprintf(stderr, "Error: add() expects 2 integer arguments only");
		return 0;
	}

	return list[0].data.as_int + list[1].data.as_int;
}

/* Example usage of "Value" array (2) */
void print_list(Value* list, int n)
{
	int i;
	for(i=0; i<n; i++)
	{
		switch (list[i].type)
		{
			case TYPE_INT:    	printf("int: %d\n",   list[i].data.as_int);    break;
	        case TYPE_FLOAT:  	printf("float: %f\n", list[i].data.as_float);  break;
	        case TYPE_STRING: 	printf("str: %s\n",   list[i].data.as_string); break;
			case TYPE_BOOL:		printf("bool: %d\n",  list[i].data.as_bool);   break;
    	}
	}
}


/* Call arbitrary function */
void call(command f, Value * list, int n)
{

}

void cmd_parse(char *str)
{
	int i;
	int word_count;
	char* str_copy;
	char* temp;
	char** words;
	int index;

	if (str == NULL)
		return;

	/* Count words to allocate proper array size */
	word_count = 0;
	str_copy = malloc(strlen(str) + 1);
	strcpy(str_copy, str);

	temp = strtok(str_copy, " \t\n");
	while (temp != NULL)
	{
		word_count++;
		temp = strtok(NULL, " \t\n");
	}
	free(str_copy);

	if (word_count == 0)
		return;

	/* Allocate array to hold words */
	words = malloc((word_count + 1) * sizeof(char *));

	str_copy = malloc(strlen(str) + 1);
	strcpy(str_copy, str);

	/* Split string into words */
	index = 0;
	temp = strtok(str_copy, " \t\n");
	while (temp != NULL)
	{
		words[index] = malloc(strlen(temp) + 1);
		strcpy(words[index], temp);
		index++;
		temp = strtok(NULL, " \t\n");
	}
	words[index] = NULL;  /* Null-terminate the array */
	free(str_copy);

	/* TODO: Use the words array */
	for(i = 0; i < word_count; i++)
	{
		printf("Token %d: %s\n", i, words[i]);
	}










	/* Free allocated memory */
	for (i = 0; i < word_count; i++)
	{
		free(words[i]);
	}
	free(words);
}

void login(char *username, char *password)
{

}

void reg(void)
{

}
