#define _XOPEN_SOURCE 500
#include "utils.h"
#include "string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void print_Value(Value x)
{
	switch (x.type)
	{
		case TYPE_INT:
		printf("%d", x.data.as_int);
		break;
		
		case TYPE_BOOL:
		printf("%d", x.data.as_int);
		break;
		
		case TYPE_STRING:
		printf("%s", x.data.as_string);
		break;

		case TYPE_FLOAT:
		printf("%f", x.data.as_float);
		break;
		
		case TYPE_VOID:
		break;

		default:
		return;
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


int parse_config_file(const char * filepath, Config * config)
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

int delete_key_config(const char *key_d, Config *config)
{
	int i;
	for (i = 0; i < config->count; i++) 
		if (strcmp(config->entries[i].key, key_d) == 0) 
		{
			/* Shift everything after i one slot to the left */
			int remaining = config->count - i - 1;
			if (remaining > 0)
				memmove(&config->entries[i], &config->entries[i + 1], remaining * sizeof(ConfigEntry));
			config->count--;
			return 0; /* found and deleted */
		}
	return -1; /* key not found */
}

char *get_config_value(const Config * config, const char * key)
{
	int i;
	if (config == NULL || key == NULL) return NULL;
	if (config->count == 0) return NULL;

	for (i = 0; i < config->count; i++) {
		if (strcmp(config->entries[i].key, key) == 0) {
			return (char *)config->entries[i].value;
		}
	}
	return NULL;
}

int dump_config_to_file(const Config config, const char * path)
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

int path_exists(const char * path)
{
	struct stat buffer;

	if (path == NULL)
		return 0;

	return (stat(path, &buffer) == 0);
}

int is_file(const char * path) {
	struct stat path_stat;

	if (stat(path, &path_stat) != 0) {
		return 0;
	}

	return S_ISREG(path_stat.st_mode);
}

#include <dirent.h>

char ** list_files_indir(const char * dir, int * n)
{
	int i = 0;
	struct dirent *entry;
	struct stat st;
	
	DIR *dp = opendir(dir); 
	char path[1024];
	
	char ** list = malloc(sizeof(char *));
	
	if (dp == NULL) 
	{
		perror("list_files_indir()");
		printf("path: %s\n", dir);
		*n = 0;
		return NULL;
	}

	i=0;
	while ((entry = readdir(dp)) != NULL) 
	{
		sprintf(path, "%s/%s", dir, entry->d_name);
		if(stat(path, &st) == -1)
		{
			perror("stat()");
			return NULL;
		}

		if(S_ISREG(st.st_mode))
		{
			list = realloc(list, (i+1) * sizeof(char *));
			list[i] = malloc(256 * sizeof(char));
			strcpy(list[i], entry->d_name);
			i++;
		}
	}
	
	closedir(dp);
	*n = i;
	return list;	
}

#define INDENT_SPACES 2
#define EXT_SUFFIX ".txt" /* this wouldve been a lot easier if i didnt have to do this.. */
void generate_file_structure(const char *role, const char *username)
{
	FILE * file , *out_file = NULL;
	char line[2048]; /* max length in a line of a file is 2048, by POSIX standard */
	char match[MAX_CONFIG_KEY_LEN];
	int role_found = 0;
	char path[256] = "\0";
	int in_path = 0, in_type = 0, in_cont = 0, in_cont_KV = 0;
	char * type;
	Config cfg_orig;

	
	file = fopen("./file_struct.yaml.txt", "r");
	if(file == NULL)
	{
		perror("generate_file_structure()");
		return;
	}

	/*check if theres top level key for role */
	while(fgets(line, sizeof(line), file) != NULL)
	{
		sprintf(match, "%s:", role);
		if(strstr(line, match)!=NULL)
		{
			role_found = 1;
			break;
		}
	}

	if(!role_found)
	{
		printf("Role %s not found\n", role);
		return;
	}

	while(fgets(line, sizeof(line), file) != NULL)
	{
		char * start, * end;
	
		/* end at the next top level key */
		if(!isspace(line[0]))
		{
			if(out_file)
				fclose(out_file);
			return; 
		}
		
		if(!isspace(line[INDENT_SPACES]))
		{
			if(out_file)
			{
				fclose(out_file);
				out_file = NULL;
			}
			in_cont = 0;
			in_cont_KV = 0;
			in_path = 0;
		}


		if(isempty(line))
			continue;

		if(out_file && in_cont)
		{
			string_trim(line);
			fprintf(out_file, "%s\n", line);
			continue;
		}

		if(out_file && in_cont_KV)
		{
			char *eq, *key, *val, *origval;
			eq = strchr(line, '=');
			if(eq == NULL)
				eq = strchr(line, ':');
			if(eq == NULL)
			{
				printf("invalid KV format");
				return;
			}
			*eq = '\0';
			key = line;
			val = eq + 1;
			string_trim(key);
			string_trim(val);

			/* preserve existing values*/
			origval = get_config_value(&cfg_orig, key);
			if(origval == NULL)
				fprintf(out_file, "%s: %s\n", key, val);
			else
				fprintf(out_file, "%s: %s\n", key, origval);
			/* but fields that arent in content will be removed */
		}
		
		
		if(!in_path)
		{
			in_path = 1;
			if(strstr(line, "- path:") == NULL)
			{
				printf("Parse error: missing - path:\n");
				return;
			}
	
			start = strchr(line, '"');
			end = strchr(start + 1, '"');
			if(start == NULL || end == NULL)
			{
				printf("Parse error: need to enclose path string with \"\n");
				return;
			}
	
			*end = '\0';
			/* string at start + 1 is a format string*/
			sprintf(path, start+1, username);
			in_type = 0;
			continue;
		}

		if(!in_type)
		{
			in_type = 1;
			start = strstr(line, "type: ");
			if(start == NULL)
			{
				printf("Parse error: missing type\n");
				return;
			}
			type = start + strlen("type: ");

			string_trim(type);
			if(strcmp(type, "file") == 0)
			{
				if(!path_exists(path))
				{
					out_file = fopen(path, "w");
					if(out_file == NULL)
					{
						perror("generate_file_structure()");
						printf("attempted %s\n", path);
						return;
					}
					printf("%s created\n", path);
				}
				else 
				{
					printf("%s exists, ignoring\n", path);
				}
				
				in_cont = 1;

				if(fgets(line, sizeof(line), file) != NULL)
					if(strstr(line, "content:") == NULL)
					{
						printf("type file must be followed by \"content:\"\n");
						return;
					}
				continue;
			}
			else if(strcmp(type, "KV file") == 0)
			{
				if(path_exists(path))
				{
					printf("%s touching\n", path);
					parse_config_file(path, &cfg_orig);
					in_cont_KV = 1;
				}
				else
				{
					printf("creating %s\n", path);
					in_cont = 1;
				}
				
				out_file = fopen(path, "w");
				if(out_file == NULL)
				{
					perror("generate_file_structure()");
					printf("attempted %s\n", path);
					return;
				}

				if(fgets(line, sizeof(line), file) != NULL)
					if(strstr(line, "content:") == NULL)
					{
						printf("type file must be followed by \"content:\"\n");
						return;
					}
				continue;
			}
			else if(strcmp(type, "directory") == 0)
			{
				if(path_exists(path))
					printf("%s exists\n", path);
				else 
					#ifdef _WIN32
					mkdir(path);
					#else
					if(mkdir(path, 0755) == -1)
					{
						perror("generate_file_structure() dir %s");
						return;
					}
					#endif
				in_path = 0;
				continue;
			}
			else 
			{
				printf("unknown type \"%s\"\n", type);
				return;
			}
		}
	}
	fclose(file);
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

void wipe_stdin(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void * clean_malloc(size_t size)
{
	void * thing = malloc(size);
	if(thing == NULL)
	{
		perror("malloc()");
		exit(1);
	}
	return thing;
}

#include <string.h>
#ifndef _WIN32
#include <ftw.h>
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	if(typeflag != FTW_D) 
	{
		return remove(fpath);
	}
	return 0;
}

int rm_rf(const char *path)
{
	return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}
#endif

int append_f2f(const char *start_path, const char *end_path)
{
	int ch;
	FILE * start = fopen(start_path, "a");
	FILE * end = fopen(end_path, "r");

	if(start == NULL || end == NULL)
	{
		perror("append_f2f");
		return -1;
	}

	while((ch = getc(end)) != EOF)
		fputc(ch, start);

	fclose(start);
	fclose(end);
	return 0;
}