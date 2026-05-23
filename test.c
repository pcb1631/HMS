#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/utils.h"
#include "utils/tui.h"

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

#define OPTIONS 64

int main(void)
{
    char ** options;
    const char ** options_p;
    int i;    
    char temp[10];

    options = (char **)malloc(OPTIONS * sizeof(char *));
    for(i=0; i<OPTIONS; i++)
    {
        options[i] = (char *)malloc(64 * sizeof(char));
        sprintf(options[i], "Option %d", i);
    }

    options_p = (const char **)options;
    simple_select(COLOR_BLUE, COLOR_BLACK, OPTIONS, options_p, temp, "pretext");

    return 0;
}
