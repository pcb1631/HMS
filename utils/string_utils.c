#include "string_utils.h"
#include "utils.h"
#ifndef _WIN32
	#include <ncurses.h>
#else
    #include <ncursesw/ncurses.h>
#endif
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

void free_LineLL(LineNode * Node)
{
	LineNode * temp;
	while(Node != NULL)
	{
		temp=Node;
		Node = Node->next;
		free(temp->str);
		free(temp);
	}
}

void delete_node_LineLL(LineNode * Node)
{
	LineNode *Prev = Node->prev, *Next = Node->next;

	if(Prev != NULL)
		Prev->next = Next;
	if(Next != NULL)
		Next->prev = Prev;

	free(Node->str);
	free(Node);
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

	LineNode * Head = NULL;
	LineNode * Node = new_LineNode();
	LineNode * Prev = NULL;
	if(file == NULL)
	{
		free(Node);
		perror("text_editor()");
		return NULL;
	}

	line = clean_malloc(LENGTH_PARSE_LIMIT * sizeof(char));
	i=0;
	while((ch = getc(file)) != EOF)
	{
		if(ch == '\r') /*fuck off */
			continue;
		
		if((i % LENGTH_PARSE_LIMIT == 0) && i!=0)
			realloc_string(&line, i+LENGTH_PARSE_LIMIT);

		if(ch == '\n')
		{
			lines_n++;
			Node->prev = Prev;
			Node->str = line;
			Node->str_n = i;
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

		line[i] = ch;
		i++;
	}

	if (i > 0) /* if i isnt 0, that means there wasn't a newline */
	{
		if((i % LENGTH_PARSE_LIMIT == 0) && i!=0)
			realloc_string(&line, i+LENGTH_PARSE_LIMIT);
		line[i] = '\0';

		Node->str = line;
		Node->str_n = i;

		if(Prev != NULL)
			Node->prev = Prev;
		if(first)
			Head = Node;

		lines_n++;
	}else
	{
		free(line);
		free(Node);
		if(Prev != NULL)
			Prev->next = NULL;
	}

	fclose(file);
	*lines_out = lines_n;
	return Head;
}

void dump_LineLL_to_file(LineNode * Head, const char * path)
{
	FILE * file = fopen(path, "w");
	if(file == NULL)
	{
		perror("dump_LineLL_to_file()");
		return;
	}

	if(Head == NULL)
	{
		fprintf(stderr, "dump_LineLL_to_file() head is null");
		return;
	}

	while(Head != NULL)
	{
		if(Head->next != NULL)
			fprintf(file, "%s\n", Head->str);
		else
			fprintf(file, "%s", Head->str);
		Head = Head->next;
	}

	printf("successfully saved to %s\n", path);
	fclose(file);
}

void realloc_string(char ** s, int n)
{
	char * new = realloc(*s, n * sizeof(char));
	if(new == NULL)
	{
		endwin();
		perror("realloc_string()");
		exit(1);
	}
	else
	{
		*s = new;
	}
}

int count_digits(int n)
{
	int cnt = 0;
	if(n == 0)
		n = 1;
	else
		while(n != 0)
		{
			n /= 10;
			cnt++;
		}

	return cnt;
}

#define min(a,b) ((a) < (b) ? (a) : (b))

/* took this directly from wikipedia hehe*/
int levenshtein_distance(const char* s, const char* t, int m, int n)
{
    int * v0 = malloc((n + 1) * sizeof(int));
    int * v1 = malloc((n + 1) * sizeof(int));
    int * swap;
    int i;
    int ii;
    int result;

    if(v0 == NULL OR v1 == NULL)
    {
        fprintf(stderr, "malloc fail levenshtein_distance()\n");
        exit(1);
    }

    for(i = 0; i<n+1; i++)
        v0[i] = i;

    for(i=0; i<m; i++)
    {
        v1[0] = i+1;

        for(ii=0; ii<n; ii++)
        {
            int del_cost = v0[ii+1]+1;
            int insert_cost = v1[ii] + 1;
            int sub_cost;

            if (s[i]==t[ii])
                sub_cost = v0[ii];
            else
                sub_cost = v0[ii]+1;

            v1[ii + 1] = min(min(del_cost, sub_cost), insert_cost);
        }

        swap = v0;
        v0 = v1;
        v1 = swap;
    }

    result = v0[n];
    free(v0); free(v1);
    return result;
}
