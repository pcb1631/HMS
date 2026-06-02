@echo off
rem Build script for HMS on Windows (uses gcc)
setlocal

set "OUTPUT=main.exe"
set "CC=gcc"
set "CFLAGS=-ansi -pedantic -Wextra -Wall -g"
set "DEFINES=-D_WIN32 -DNCURSES_STATIC"
set "INCLUDES=-Ideps/crypt_blowfish"
set "LIBS=-ladvapi32 -lncursesw"
set "SRCS=main.c utils/utils.c utils/string_utils.c utils/logins.c utils/tui.c utils/win_api.c user_commands/commands.c user_commands/bookings.c utils/globals.c deps/crypt_blowfish/wrapper.c deps/crypt_blowfish/crypt_blowfish.c deps/crypt_blowfish/crypt_gensalt.c"

echo Compiling %OUTPUT%...
%CC% %SRCS% %INCLUDES% -o %OUTPUT% %CFLAGS% %DEFINES% %LIBS%
if errorlevel 1 (
	echo Build failed with exit code %errorlevel%.
	endlocal
	exit /b %errorlevel%
)

echo Build succeeded.
endlocal
