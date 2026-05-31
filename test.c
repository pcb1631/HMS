// Source - https://stackoverflow.com/a/78084698
// Posted by greg spears, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-31, License - CC BY-SA 4.0

#include <ncurses/curses.h>
#include <windows.h>
#include <stdio.h>
#include <ncursesw/curses.h>

HANDLE hConOut = NULL;


/*------------------------------------------------------------
 
 getConsoleOutputHandle()

 There are simpler ways to get the console handle, but they 
 arguably lack precision.

*------------------------------------------------------------*/
HANDLE getConsoleOutputHandle(void)
{
SECURITY_ATTRIBUTES sa;

    if(!hConOut)
    {
        /* First call -- get the window handle one time and save it*/
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        /* Using CreateFile we get the true console handle", avoiding any redirection.*/
        hConOut = CreateFile( TEXT("CONOUT$"),
             GENERIC_READ | GENERIC_WRITE,
             FILE_SHARE_READ | FILE_SHARE_WRITE,
             &sa, OPEN_EXISTING, (DWORD)0, (HANDLE)0 );
    }
    if(!hConOut) /* actually, this is a bad error, let your app handle the error as needed*/
    {
        printf("getConsoleOutputHandle(): failed to get Console Window Handle\n");
        return NULL;
    }
    return hConOut;
}


/*----------------------------------------------------------------------------------------
 
    check_console_window_resize_event()

    Params: COORD to return new window size
    Returns: TRUE if the console window has changed size.  FALSE if not.

    USAGE: Best practice is to call the function repeatedly from your main application 
    loop.   Preferably a place where the function can be called several times per second 
    throughout the program's run time.

    DATE:           Reason:
    2022.10.05      Created -- placed in public domain
*----------------------------------------------------------------------------------------*/
int check_console_window_resize_event(COORD *info)
{
    static short old_screen_w=0, old_screen_h=0;
      /* variables declared static hold their value between function calls.*/
    short current_screen_w, current_screen_h;
    int window_resized = FALSE;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    hConOut = getConsoleOutputHandle();
    if(!hConOut)
        return FALSE;

    if(!GetConsoleScreenBufferInfo( getConsoleOutputHandle(), &csbi ))
    {
        //printf("check_console_window_resize_event(): GetConsoleScreenBufferInfo() FAILED!!  %s\n", __FILE__);
        return FALSE;
    }
    current_screen_w  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    current_screen_h  = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    if(!old_screen_w && !old_screen_h)
    {
        /* Execution comes here if this is first time this function is called.  
        ** Initialize the static variables and bail...*/
        old_screen_w = current_screen_w;
        old_screen_h = current_screen_h;
        return FALSE;
    }

    /* At last the real work of this function can be realized...*/
    if(current_screen_w != old_screen_w || current_screen_h != old_screen_h)
    {
        old_screen_w = current_screen_w;
        old_screen_h = current_screen_h;
        window_resized = TRUE;
        info->X = current_screen_w;
        info->Y = current_screen_h;
        //printf("check_console_window_resize_event(): new screenwidth:  %d new screenheight:  %d", current_screen_w, current_screen_h);
    }
    return window_resized;
}


/*---------------------------------------------------------------------------------------------
   main()
*--------------------------------------------------------------------------------------------*/
int main(void) 
{
    BOOL user_quit = FALSE;
    COORD winsize;
	raw();
    
    while(!user_quit)
    {
    
        if(check_console_window_resize_event(&winsize))
        {
            printf("Hey! You resize me? I'm really %dx%d now?\n", winsize.X, winsize.Y);
        }
        
    }
    return 0;
}
