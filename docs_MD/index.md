\mainpage Hospital Management System

**For anyone viewing the PDF, docs are also hosted at https://pcb1631.github.io/HMS/**

- \subpage deps 
- \subpage usage
  - \subpage usage_basics
  - \subpage director
  - \subpage doctors
  - \subpage nurses
  - \subpage patients
- \subpage term
- \subpage dev
- \subpage design
  - \subpage db



\section intro_sec Introduction


 The Hospital Management System (HMS for short) is an interactive program to facilitate hospital documentation, appointment booking, among other services.
 HMS is designed to run in every major OS, as it is written purely in ANSI C



 
\section installation Installation / Compiling
HMS was primarily developed on unix-like systems, so it is reccomended to use WSL if you're on Windows.

(Don't use git if you are using the zip file)

Clone the repo:
```bash
git clone https://github.com/pcb1631/HMS
```

Add git submodules: 
```bash
git submodule update --init --recursive
```

## Linux/MacOS
ncurses is shipped with most linux distros, while ncurses 6.x is shipped with MacOS Sonoma and later

1. make
  - make should come with xcode if you're using MacOS. If not, install make with homebrew
```bash
brew install make
```


Use the provided Makefile to compile the project:
```bash
make
```

When making changes in files other than main.c, delete the binary and run make again:
```bash
make clean; make
```


  

## Windows
Install `ncurses` from the UCRT64 environment (MSYS2):
```bash
pacman -S mingw-w64-ucrt-x86_64-ncurses
```
Then, use the Makefile to compile:
```bash
./make
```

TODO: Add subpages in usage.md on how to use TUI. Command TUI, simple TUI, and time tui

\section assumptions Assumptions
It is assumed that:

-# You are using Windows or unix-like system
-# You have the programs used in \ref installation
	- make (replaced with Make.bat for windows)
	- MSYS2 for UCRT64, for ncurses
	- gcc
	- git 
-# You have followed all the steps in \ref installation
-# The program is compiled by yourself with the makefile
-# The program is located in the same dir as the db folder (users/)
-# You are using a terminal tested by us in \ref term. (At the very least, the linux tty)

-# Users
	- have eyes and hands
	- know that CTRL+C and CTRL+D can cancel running commands or exit the program
	- know that the password is actually hidden and the program is still running
-# Admins / IT 
	- have basic understanding of C and the terminal
	- absolutely know how the plaintext files are formatted
	-	read the documentation and understood how to
		-	change the configuration of the program
		- use the framework to add commands
