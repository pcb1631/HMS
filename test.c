#include "tui.h"
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

int main(void)
{
    time_t t;
    char buf[80];
    struct tm * ts;
    time(&t);
    t = time_picker("pickme", COLOR_BLUE, COLOR_BLACK, t);
    ts = localtime(&t);



    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
    printf("%s\n", buf);
    return 0;
}
