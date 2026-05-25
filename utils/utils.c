#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>



Value make_int(int x)
{
    Value v;
    v.type = TYPE_INT;
    v.data.as_int = x;
    return v;
}
Value make_float(float x)
{
    Value v;
    v.type = TYPE_FLOAT;
    v.data.as_float = x;
    return v;
}
Value make_string(char * s)
{
    Value v;
    v.type = TYPE_STRING;
    strcpy(v.data.as_string, s);
    return v;
}


Value add(Value* list)
{
    printf("Result: %d", list[0].data.as_int + list[1].data.as_int);
    getchar();
	return make_int(list[0].data.as_int + list[1].data.as_int);
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

	if(strcmp(perm, f.permission) != 0)
	{
		fprintf(stderr, "Permission denied (current user perm: %s, command perm: %s)\n", perm, f.permission);
		return err;
	}

	if(f.n_of_args == 0)
	    return ((Value (*)(void))f.ptr)();

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

	return ((Value (*)(Value *))f.ptr)(args);
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

void trim_trailing(char *str)
{
    int len;

    if (str == NULL)
        return;

    len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        len--;
    }
    str[len] = '\0';
}

int parse_config_file(const char *filepath, Config *config)
{
    FILE *fp;
    char line[512];
    char *eq;
    char *key_str;
    char *val_str;

    if (config == NULL)
    {
        return -1;
    }

    config->count = 0;

    fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        return -1;
    }

    while (fgets(line, sizeof(line), fp))
    {
        eq = strchr(line, '=');
        if (eq == NULL)
        {
            eq = strchr(line, ':'); /* Fallback to colon if used as delimiter */
        }

        /* scanf is not used here to facilitate multiple delim chars */

        if (eq != NULL)
        {
            *eq = '\0'; /* to mark the end of key */
            key_str = line;
            val_str = eq + 1; /* eq is the location of =, therefore the value string is at eq+1 */

            string_trim(key_str);
            string_trim(val_str);

            strncpy(config->entries[config->count].key, key_str, MAX_CONFIG_KEY_LEN - 1);
            config->entries[config->count].key[MAX_CONFIG_KEY_LEN - 1] = '\0';

            strncpy(config->entries[config->count].value, val_str, MAX_CONFIG_VAL_LEN - 1);
            config->entries[config->count].value[MAX_CONFIG_VAL_LEN - 1] = '\0';

            config->count++;
            if (config->count >= MAX_CONFIG_ENTRIES)
            {
                break;
            }
        }
    }

    fclose(fp);
    return 0;
}

const char *get_config_value(const Config *config, const char *key)
{
    int i;
    if (config == NULL || key == NULL) return NULL;

    for (i = 0; i < config->count; i++) {
        if (strcmp(config->entries[i].key, key) == 0) {
            return config->entries[i].value;
        }
    }
    return NULL;
}

int dump_config_to_file(const Config config, const char* path)
{
	FILE * file;
	int i;

	file = fopen(path, "w");
	if(file == NULL)
	{
		perror("fopen");
		return -1;
	}

	for(i=0; i<config.count; i++)
		fprintf(file, "%s: %s\n", config.entries[i].key, config.entries[i].value);

	printf("Dumped config to %s\n", path);

	if(fclose(file) !=0 )
		perror("fclose");

	file = NULL;
	return 0;
}

int path_exists(const char *path)
{
    struct stat buffer;

    if (path == NULL)
        return 0;

    return (stat(path, &buffer) == 0);
}

void generate_file_structure(const char *role, const char *username)
{
    FILE *fp;
    char line[512];
    int in_role = 0;
    char full_path[512];
    char role_header[128];
    FILE *out_fp = NULL;
    char *start;
    char *end;
    char *key;
    int len;

    sprintf(full_path, "./users/%s/", username);
    sprintf(role_header, "%s:", role);

    fp = fopen("file_struct.yaml.txt", "r");
    if (!fp) return;

    while (fgets(line, sizeof(line), fp))
    {
        if (line[0] != ' ' && line[0] != '\n' && line[0] != '\r')
        {
            if (strncmp(line, role_header, strlen(role_header)) == 0)
                in_role = 1;
            else
                in_role = 0;

            if (out_fp)
            {
                fclose(out_fp);
                out_fp = NULL;
            }
            continue;
        }

        if (in_role)
        {
            if (strstr(line, "- path:"))
            {
                if (out_fp)
                {
                    fclose(out_fp);
                    out_fp = NULL;
                }
                start = strchr(line, '"');
                if (start)
                {
                    end = strchr(start + 1, '"');
                    if (end)
                    {
                        *end = '\0';
                        sprintf(full_path, start + 1, username);
                        len = strlen(full_path);

                        if (path_exists(full_path))
                        {
                            printf("%s exists\n", full_path);
                            continue;
                        }

                        if (len > 0 && full_path[len-1] == '/')
                        {
                            #ifdef _WIN32
                            mkdir(full_path);

                            #else
                            mkdir(full_path, 0755);

                            #endif
                        }
                        else
                        {
                            out_fp = fopen(full_path, "w");
                        }
                    }
                }
            }
            else if (strstr(line, "type: directory"))
            {
                /* Handled by trailing slash */
            }
            else if (strstr(line, "type: file") || strstr(line, "content:"))
            {
                /* Skip */
            }
            else if (out_fp && strstr(line, ":"))
            {
                key = line;
                while (*key == ' ') key++;
                if (*key != '\n' && *key != '\r' && *key != '\0')
                {
                    fputs(key, out_fp);
                }
            }
        }
    }

    if (out_fp)
    {
        fclose(out_fp);
    }

    fclose(fp);
    printf("OK\n");
}

command_node* new_command_node(command f)
{
    command_node* node = (command_node*)malloc(sizeof(command_node));
    if (node == NULL)
    {
        fprintf(stderr, "Failed to allocate command node\n");
        exit(1);
    }

    node->f=f;
    node->next=NULL;
    return node;
}

void free_cmd_list(command_node* head)
{
    command_node* temp;
    while(head != NULL)
    {
        temp = head;
        head = temp->next;
        free(temp);
    }
}

const char * get_utype(const char * username)
{
    Config user;
    char path[64] = "";
    sprintf(path, "./users/%s/profile.txt", username);

    if(parse_config_file(path, &user) == -1)
    {
        fprintf(stderr, "Config file is empty or doesn't exist: %s\n", username);
        return "";
    }

    return get_config_value(&user, "USERTYPE");
}

#define min(a,b) ((a) < (b) ? (a) : (b))

/* took this directly from wikipedia lmfao */
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

void wipe_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
