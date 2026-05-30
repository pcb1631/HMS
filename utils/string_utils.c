#include "string_utils.h"
#include "utils.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void string_pop(char * str, int pos)
{
	/* replace current char with the char after, stop at /0 */
	int i = pos;
	while(str[i] != '\0')
	{
		str[i] = str[i+1];
		i++;
	}
}

void string_insert(char * str, char ins, int pos)
{
	/* replace current char with the char before until pos */
	int n = strlen(str);
	int i = n;
	str[n + 1] = '\0';
	while(i > pos)
	{
		str[i] = str[i-1];
		i--;
	}
	str[pos] = ins;
}

void string_trim(char * s)
{
	trim_trailing(s);
	while(isspace(s[0]))
	{
		string_pop(s, 0);
	}
}

void trim_trailing(char *str)
{
    int len;

    if (str == NULL)
        return;

    len = strlen(str);
    while (len > 0 && isspace(str[len - 1]))
        len--;

    str[len] = '\0';
}

LineNode* new_LineNode(void)
{
	LineNode* node = (LineNode*)malloc(sizeof(LineNode));
	if(node == NULL)
	{
		perror("new_LineNode()");
		exit(1);
	}
	node->next=NULL;
	node->prev=NULL;
	node->str=NULL;
	return node;
}

void free_LineLL(LineNode * Head)
{
	LineNode * Node = Head;
	LineNode * temp;
	while(Node != NULL)
	{
		temp = Node->next;
		free(Node->str);
		free(Node);
		Node = temp;
	}
}

#define LENGTH_PARSE_LIMIT 10

LineNode* file_to_LL_lines(const char * path, int * lines_out)
{
	int lines_n = 0;
	int i;
	int ch;
	int first = 1;
	char* line;
	FILE* file = fopen(path, "r");

	LineNode * Head;
	LineNode * Node = new_LineNode();
	LineNode * Prev = NULL;
	if(!is_file(path))
	{
		fprintf(stderr, "file_to_strings(): \"%s\" is not a file\n", path);
		fclose(file);
		return NULL;
	}

	if(file == NULL)
	{
		perror("text_editor()");
		fclose(file);
		return NULL;
	}

	line = clean_malloc(LENGTH_PARSE_LIMIT * sizeof(char));
	i=0;
	while((ch = getc(file)) != EOF)
	{
		if(ch == '\n')
		{
			Node->prev = Prev;
			Node->str = line;
			Node->str_n = strlen(line);
			Node->next = new_LineNode();
			if(first)
			{
				Head = Node;
				first = 0;
			}
			Prev = Node;
			Node = Node->next;

			line[i] = '\0';

			i=0;
			line = malloc(LENGTH_PARSE_LIMIT * sizeof(char)); /* address to string is stored in node, so make a new one for the next node */


			continue;
		}

		if((i % LENGTH_PARSE_LIMIT == 0) && i!=0)
			realloc_string(&line, i+LENGTH_PARSE_LIMIT);

		line[i] = ch;
		i++;
	}
	Node->str = line;
	Node->prev = Prev;

	fclose(file);

	*lines_out = lines_n;
	return Head;
}

char ** file_to_strings(const char * path, int * lines_out)
{
	int lines_n = 1;
	int i;
	char ** line;
	int ch;
	FILE * file = fopen(path, "r");
	if(!is_file(path))
	{
		fprintf(stderr, "file_to_strings(): \"%s\" is not a file\n", path);
		fclose(file);
		return NULL;
	}

	if(file == NULL)
	{
		perror("file_to_strings()");
		fclose(file);
		return NULL;
	}

	line = malloc(lines_n * sizeof(char *));
	line[0] = malloc(LENGTH_PARSE_LIMIT * sizeof(char));

	i = 0;
	while( ( ch = getc(file) ) != EOF )
	{
		if( (i % LENGTH_PARSE_LIMIT == 0) && (i > 0) )
		{
			char * temp;
			temp = realloc(line[lines_n-1], (i + LENGTH_PARSE_LIMIT) * sizeof(char));
			if(temp != NULL)
			{
				line[lines_n-1] = temp;
			}
			else
			{
				perror("text_editor() failed to realloc");
				fclose(file);
				return NULL;
			}
		}

		if(ch == '\n') /* I'm just gonna assume nobody is using pre OS X, and I dont care if windows uses CRLF*/
		{
			char ** temp;
			line[lines_n-1][i] = '\0';
			i=0;

			lines_n++;

			/* this makes space for one more char pointer, but now that pointer points to garbage
			 * so need to malloc for the pointer
			 */
			temp = realloc(line, lines_n * sizeof(char *));
			if(temp != NULL)
			{
				line = temp;
			}
			else
			{
				perror("text_editor() failed to realloc");
				endwin();
				fclose(file);
				return NULL;
			}

			line[lines_n-1] = malloc((i + LENGTH_PARSE_LIMIT) * sizeof(char));
			continue;
		}

		line[lines_n-1][i] = ch;
		i++;
	}

	fclose(file);

	line[lines_n-1][i] = '\0';

	*lines_out = lines_n;

	return line;
}

void realloc_string(char ** s, int n)
{
	char * new = realloc(*s, n * sizeof(char));
	if(new == NULL)
	{
		perror("realloc_string()");
		endwin();
		exit(1);
	}
	else
	{
		*s = new;
	}
}
