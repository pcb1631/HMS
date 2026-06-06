#ifndef WIN_API_HMS
#define WIN_API_HMS
#ifdef _WIN32
#include <windows.h>

HANDLE getConsoleOutputHandle(void);

int check_console_window_resize_event(COORD *info);
#endif

struct dummy;

#endif