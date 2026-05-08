#include "tui.h"
#include <stdio.h>
#include <time.h>
#include "utils.h"

#ifdef _WIN32
    #include <ncurses/curses.h>
#else
    #include <ncurses.h>
#endif

int main(int argc, char ** argv)
{
    char * username = "spoingus";
    generate_file_structure("Doctor", username);
}
