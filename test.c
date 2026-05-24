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
    int i;
    for(i=0; i<argc; i++)
    {
        printf("%s\n", argv[i]);
        printf("%s\n", getenv("TERM"));
    }

    return 0;
}
