\mainpage Hospital Management System
- \subpage deps 
- \subpage usage
  - \subpage usage_basics
  - \subpage director
  - \subpage doctors
  - \subpage nurses
  - \subpage patients
- \subpage term

\section intro_sec Introduction
 The Hospital Management System (HMS for short) is an interactive program to facilitate hospital documentation, appointment booking, among other services.
 HMS is designed to run in every major OS, as it is written purely in ANSI C



 
\section installation Installation / Compiling
HMS was primarily developed for linux, so it is reccomended to use WSL if you're on Windows.

Clone the repo:
```bash
git clone https://github.com/pcb1631/HMS
```

1. crypt_blowfish 
  - crypt_blowfish is the library we use for password hashing. 
Add it as a git submodule: 
```bash
git submodule update --init --recursive
```

## Linux/MacOS
ncurses is shipped with most linux distros, while ncurses 5.x is shipped with MacOS

1. make
  - make should come with xcode if you're using MacOS. If not, install make with homebrew
```bash
brew install make
```


Use the provided Makefile to compile the project:
```bash
make
```

When making changes in files other than main.c:
```bash
make clean
```
Then run make again.


  

## Windows
Install `ncurses` from the UCRT64 environment (MSYS2):
```bash
pacman -S mingw-w64-ucrt-x86_64-ncurses
```
Then, use the Makefile to compile:
```bash
./make
```
