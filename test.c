#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/utils.h"

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#define OPTIONS 64

int main(int argc, char ** argv)
{
	Config cfg;
	parse_config_file("temp1.txt", &cfg);

	dump_config_to_file(cfg, "temp2.txt");

    return 0;
}
