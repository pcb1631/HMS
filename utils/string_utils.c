#include "string_utils.h"
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
