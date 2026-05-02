#include "tui.h"
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

int main(int argc, char ** argv)
{
    int i;
    for(i=0; i<argc; i++)
        printf("%s\n", argv[i]);
    return 0;
}
