\page deps Dependencies, third-party libraries of HMS

\section uthash_h uthash.h
uthash is a header-only library, which provides hash table lookup for user credentials. We chose this library for its simplicity

\section crypt_blowfish crypt_blowfish
This library provides password hashing for security. 
\n You will see a warning emmited for crypt_gensalt_ra when compiling for ANSI C, but we won't be using the function.
\n git submodule


\section doxygen-awesome-css doxygen-awesome-css
This is only for HTML docs.
\n git submodule


\section ncurses ncurses
ncurses allows us to make TUI libraries to make the program more interactive and user-friendly. 
\n It is shipped with macOS (ncurses 5.x) and most linux distros.

**However, you need to install ncurses with UCRT64 on Windows.**

For compatibility, we only use the curses.h subset, ncursesw for wide char support on Windows.
