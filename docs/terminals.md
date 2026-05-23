\page term Compatible terminals with HMS

Every terminal has different behaviour when running HMS, due to the nature of ncurses and the way terminals handle newlines (When you press enter when HMS is prompting for input). 

As such we have compiled a list of terminals we tested and their behaviour.

**In case the program crashes, ncurses will not be able to call endwin(), and the terminal will most likely be broken because it can't revert ANSI escape codes. If this happens:**
```bash
reset
```
**This will reset the terminal to it's usual state.**

1. Zed Editor terminal
  - RGB is supported, but won't display correctly
  - Will need to prepend \\n to scanf format string, if scanf is used consecutively. Or use fgets so you can trim newlines
  - debug terminal broken, first ncurses menu will not render properly, and window borders won't use the correct char
  - If debugging, SIGINT (CTRL+C) during an ncurses menu will fill up your RAM very quickly 
  - it works at least 
2. vscode terminal
  - RGB is supported 
  - Won't render black background
3. cmd/Powershell
  - RGB isn't supported
  - Ugly
4. Ghostty
  - Good
  - Is available on Linux and MacOS
  - It might crash if you highlight with your cursor for some reason
5. kitty
  - Perfect
6. xterm
  - No
7. Linux TTY
  - RGB isn't supported
8. ratty
  - no

- macOS
  - Behaviour between every terminal emulator seems similar, might render colors differently
  - RGB is supported