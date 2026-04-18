#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


Value make_int(int x)          	{ Value v; v.type = TYPE_INT;    v.data.as_int    = x; return v; }
Value make_float(float x)      	{ Value v; v.type = TYPE_FLOAT;  v.data.as_float  = x; return v; }
Value make_string(char * s)		{ Value v; v.type = TYPE_STRING; v.data.as_string = s; return v; }


Value add(Value* list)
{
	return make_int(list[0].data.as_int + list[1].data.as_int);
}

Value meow(Value* list)
{
	printf("Meow\n");

	return make_int(1);
}

Value cat(Value * list)
{
    printf("ᓚ₍ ^. .^₎\n");
    return make_int(1);
}


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
			case TYPE_VOID:		printf("void \n"); 							   break;
    	}
	}
}


Value call(command f, Value * args, int n_of_args, char * perm)
{
	int i;
	Value err = make_int(0);

	if(perm != f.permission)
	{
		fprintf(stderr, "Permission denied (current user perm: %s, command perm: %s)\n", perm, f.permission);
		return err;
	}

	if(n_of_args != f.n_of_args && n_of_args != -1)
	{
		fprintf(stderr, "Command %s takes %d args, but was given %d\n", f.alias, f.n_of_args, n_of_args);
		return err;
	}

	/* Check types */
	for(i=0; i<n_of_args; i++)
	{
	    if(f.arg_type[i] == TYPE_VOID)
			break;
		if(f.arg_type[i] != args[i].type)
		{
			fprintf(stderr, "argument %d type mismatch for command %s\n", i, f.alias);
			return err;
		}
	}

	return ((Value (*)(Value *, int))f.ptr)(args, n_of_args);
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

int string_trim(char * s)
{
        int len;
        int start = 0, end;
        int has_space = 0;
        int j;

        len = strlen(s);
        if (len > 0) end = len - 1;
        else end = 0;

        while (start < len && isspace((unsigned char)s[start])) start++;
        while (end >= start && isspace((unsigned char)s[end])) end--;

        len = end - start + 1;
        for (j = 0; j < len; j++) {
            s[j] = s[start + j];
            if (isspace((unsigned char)s[j])) has_space = 1;
        }
        s[len] = '\0';

        if (has_space || len == 0)
        {
            return 0;
        }else {
            return 1;
        }
}
