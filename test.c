#include "utils/utils.h"
#include <ncurses.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	Config cfg;
	int i=0;
	parse_config_file("temp1.txt", &cfg);
	for(i=0; i<cfg.count; i++)
	{
		printf("%s, %s\n", cfg.entries[i].key, cfg.entries[i].value);
	}
	return 0;
}