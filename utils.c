#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

	/* Split string into words */
	str_copy = malloc(strlen(str) + 1);
	strcpy(str_copy, str);

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
