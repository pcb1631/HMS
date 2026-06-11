#include "win_api.h"
#ifdef _WIN32
#include <consoleapi2.h>
#include <minwindef.h>
#include <processenv.h>
#include <stdio.h>
#include <windows.h>

/**
// Source - https://stackoverflow.com/a/78084698
// Posted by greg spears, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-02, License - CC BY-SA 4.0
*/
HANDLE getConsoleOutputHandle(void)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hConOut;

	/* First call -- get the window handle one time and save it*/
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	/* Using CreateFile we get the true console handle", avoiding any redirection.*/
	hConOut = CreateFile( TEXT("CONOUT$"),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&sa, OPEN_EXISTING, (DWORD)0, (HANDLE)0 );
	
	if(!hConOut) /* actually, this is a bad error, let your app handle the error as needed*/
	{
		printf("getConsoleOutputHandle(): failed to get Console Window Handle\n");
		return NULL;
	}
	return hConOut;
}

/** 
// Source - https://stackoverflow.com/a/78084698
// Posted by greg spears, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-02, License - CC BY-SA 4.0
*/
int check_console_window_resize_event(COORD *info)
{
	static short old_screen_w=0, old_screen_h=0;
	/* variables declared static hold their value between function calls.*/
	short current_screen_w, current_screen_h;
	int window_resized = FALSE;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	HANDLE hConOut = getConsoleOutputHandle();
	if(!hConOut)
		return FALSE;

	if(!GetConsoleScreenBufferInfo( getConsoleOutputHandle(), &csbi ))
	{
		/*printf("check_console_window_resize_event(): GetConsoleScreenBufferInfo() FAILED!!  %s\n", __FILE__);*/
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

	if(current_screen_w != old_screen_w || current_screen_h != old_screen_h)
	{
		old_screen_w = current_screen_w;
		old_screen_h = current_screen_h;
		window_resized = TRUE;
		info->X = current_screen_w;
		info->Y = current_screen_h;
		/*printf("check_console_window_resize_event(): new screenwidth:  %d new screenheight:  %d", current_screen_w, current_screen_h);*/
	}
	return window_resized;
}

int rm_rf_windows(const char* path) 
{
	SHFILEOPSTRUCTA file_op = {0};
	int len = strlen(path);
	int result;  
	/* double null termination for the api...... why?*/
	char double_null_path[512];
	double_null_path[len] = '\0';
	double_null_path[len+1] = '\0';

	snprintf(double_null_path, sizeof(double_null_path), "%s", path);

	file_op.wFunc = FO_DELETE;
	file_op.pFrom = double_null_path;
	/* FOF_NOCONFIRMATION: suppress "Are you sure?" dialogs */
	/* FOF_NO_UI: suppress progress dialogs and error popups */
	file_op.fFlags = FOF_NOCONFIRMATION | FOF_NO_UI | FOF_SILENT;

	result = SHFileOperationA(&file_op);
	return result; 
}

#endif