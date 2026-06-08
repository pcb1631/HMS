\page term Compatible terminals with HMS

Every terminal has different behaviour when running and debugging HMS/ncurses, due to the the fact that terminals don't seem to agree on one ANSI escape code standard 

As such we have compiled a list of terminals we tested and their behaviour.

**In case the program crashes, ncurses will not be able to call endwin(), and the terminal will most likely be broken because it can't revert ANSI escape codes. (e.g. if you see strange characters when you backspace or press enter)**
**If this happens:**
```bash
reset
```
**This will reset the terminal to it's usual state.**

The program already does `reset` first just in case (This stops the terminal on macOS for a couple seconds)

\section terms Terminals we tested
-# Zed Editor terminal
  - RGB is supported, but colors won't display correctly
  - Will need to prepend \\n to scanf format string, if scanf is used consecutively. Or use fgets so you can trim newlines
  - debug terminal mostly broken (But not on macOS); won't receive KEY_RESIZE 
  - Need to use `xterm-256color` for $TERM if you're debugging on macos, we have done this for you in the provided .zed folder
-# vscode terminal
  - RGB is supported 
  - Won't render black background
-# cmd/Powershell
  - RGB isn't supported
-# Ghostty
  - Good
  - Is available on Linux and MacOS
  - It might crash if you highlight with your cursor for some reason
-# kitty
  - Good
-# xterm
  - It works, supports rgb, but needs some setup;
	- Need to change $TERM to xterm-256color
		 ```bash 
			export TERM=xterm-256color
			```
	- On some distros/macOS, xterm sends `^H` on backspace, but terminals nowadays use `^?` instead. Thus `^H` might show up during scanf 
		```bash
			stty erase '^H' # sets the erase char to ^H
		```
	tip: you can do `stty -a` to view current terminal settings. Doing CTRL+V before inputting a key (or key combo) will let you see what the terminal interprets it as
-# xfce4-terminal
	- Good
-# Alacritty
	- Best performance
  - Available on Windows, Linux, and macOS

-# Linux TTY
  - Absolutely not
-# ratty
  - no

\section windows Windows
  Should have the same behaviour as Linux with WSL. 

\section mac macOS
	Behaviour between every terminal emulator is similar because they advertise themselves as `xterm-256color`

	The VT100 (1978) was the first widely successful terminal implementing ANSI standards, making it the hardware de facto standard. xterm introduced its own features like mouse tracking and configurable color pallete, which prompted xterm clones to spawn, then eventually it became the new de factor standard. (https://gpanders.com/blog/state-of-the-terminal/)
