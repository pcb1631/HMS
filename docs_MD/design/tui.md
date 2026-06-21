\page tui Text User Interfaces
We make use of the ncurses library to create TUIs, in contrast to our previous python project where we had to manually deal with ANSI escape keys.
\n This doesn't mean ncurses will abstract everything, it still requires knowledge of the terminal and how ncurses works.

The proper pattern is to use an init function (ncurses handles state externally, so we don't worry about scope), call `refresh()` every loop, and call `clear()` if doing anything that leaves characters in the terminal buffer (i.e. backspacing in text editor, the characters were never explicitly cleared)

\section windows_tui_design_caveats Setting the terminal to raw mode, and Windows quirks
**ncurses on Windows will not see CTRL+Key until you press another key, unless the render loop is non-blocking and has a timeout.**
\n non-blocking meaning will keep looping even though the user hasn't pressed a key.
\n in other words, `nodelay(1)` and `wtimeout(16)` makes `getch()` return `-1` when there is nothing in stdin


We wanted to have custom behaviour with CTRL+C such that it wouldnt outright quit the program, and instead cancel the current operation. 
\n This means we call `raw()` in \ref ncurses_init() so we get to handle CTRL+Key events ourselves.

Setting a terminal to raw mode/cbreak means that every single character is immediately sent to stdin, raw mode makes the terminal let characters through wihtout sending signals (e.g. SIGINT from CTRL+C)
\n In contrast to the default cooked mode, it only sends characters to stdin once you send newline (enter)

so the windows API equivalent to raw mode and noecho would be:

@code{.c}
DWORD mode;
GetConsoleMode(hin, &mode);
mode &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT);
SetConsoleMode(hin, mode);
@endcode


On Linux, when you press `CTRL+Q`, the PTY translates it to a single byte `0x11` and stuffs it in stdin. It's already there when ncurses calls read() on stdin
\n On Windows, input goes through the event queue of `ReadConsoleInput`(https://learn.microsoft.com/en-us/windows/console/readconsoleinput), which deal with `INPUT_RECORD` structs. keyboard events with flags for modifier keys (CTRL, SHIFT, ALT). ncurses on Windows has to:

-# Call ReadConsoleInput to consume the event queue
-# Check CTRL is held via the modifier flags
-# Manually compute the control character value (`key & 0x1F`)
-# Put the byte into its own internal buffer for getch() to return
\n ... which all take non-zero time to complete.

When the render loop is blocking or the timeout is 0, `getch()` will check its internal buffer and find nothing, because it doesn't have time at all to process the CTRL+key so the `INPUT_RECORD` is still sitting in Window's event queue, and `getch()` will still return `-1`. That is until you press another key and it suddenly comes through.

Also cmd and Powershell doesn't send `KEY_RESIZE` to the program, so we have to use the windows API and poll for the terminal size ourselves.
\n \ref check_console_window_resize_event()


\section simple_tui_design Simple TUI
\ref simple_select()

```dot
digraph simple_select {
    graph [
        rankdir=TB
        splines=ortho
        nodesep=0.5
        ranksep=0.6
        fontname="Helvetica"
        bgcolor="white"
    ]
    node [
        fontname="Helvetica"
        fontsize=11
        style=filled
        fillcolor="#f0f4ff"
        color="#3a5aad"
        penwidth=1.5
    ]
    edge [
        fontname="Helvetica"
        fontsize=10
        color="#444444"
        penwidth=1.2
    ]

    /* Terminals */
    node [shape=oval fillcolor="#1a2a5e" fontcolor=white color="#1a2a5e"]
    START [label="START"]
    END_OK [label="RETURN selection"]
    END_ERR1 [label="RETURN -1\n(no options)"]
    END_ERR2 [label="RETURN -1\n(ETX/EOT)"]

    /* Process boxes */
    node [shape=box fillcolor="#dce8ff" fontcolor=black color="#3a5aad"]
    INIT [label="ncurses_init()\ncolor_pair(1), curs_set(1)\ny_max = getmaxy(stdscr)"]
    LOOP_INIT [label="Reset leven_min = INT_MAX"]
    DRAW_PRE [label="mvprintw: pretext at row 0"]
    DRAW_OPTS [label="Draw visible options\ncentered on y_max/2\nHighlight current selection"]
    DRAW_SRCH [label="mvprintw: search bar at bottom row"]
    REFRESH [label="refresh()\nch = getch()"]
    NAV_UP [label="selection = (selection - 1 + N) % N\nclear()"]
    NAV_DOWN [label="selection = (selection + 1) % N\nclear()"]
    RESIZE_WIN [label="y_max = getmaxy(stdscr)\nclear()"]
    WIN_RESIZE [label="Update y_max\nresize_term()\nclear()"]
    CONFIRM [label="endwin()"]
    EOT_ACT [label="strcpy(selection_s, \"\")\nendwin()\nprintf(\"EOF/EOT\")"]
    SEARCH_POP [label="string_pop(search_buf)\nRecalc selection\nvia Levenshtein\nclear()"]
    SEARCH_APP [label="string_insert(search_buf, ch)\nRecalc selection\nvia Levenshtein\nclear()"]
    COPY_OUT [label="strcpy(selection_s, options[selection])"]

    /* Decision diamonds */
    node [shape=diamond fillcolor="#fff7d6" fontcolor=black color="#b07800"]
    CHK_N [label="n_of_options < 1?"]
    CHK_LOOP [label="ch == '\\n'?"]
    CHK_COVER [label="Would cover\npretext row?"]
    CHK_SEL [label="i == selection?"]
    CHK_KEY [label="Key pressed?"]
    CHK_PRINT [label="Printable\nASCII?\n(32–126)"]
    CHK_BUFLEN [label="strlen(search_buf) > 0?"]

    /* --- Graph edges --- */

    START -> INIT
    INIT -> CHK_N

    CHK_N -> END_ERR1 [label="yes"]
    CHK_N -> LOOP_INIT [label="no"]

    LOOP_INIT -> DRAW_PRE

    DRAW_PRE -> CHK_COVER [label="for each option i"]
    CHK_COVER -> DRAW_OPTS [label="no  "]
    CHK_COVER -> DRAW_SRCH [label="yes (skip)", style=dashed]
    DRAW_OPTS -> CHK_SEL
    CHK_SEL -> DRAW_OPTS [label="i++ (next option)", style=dashed]
    CHK_SEL -> DRAW_SRCH [label="all drawn"]

    DRAW_SRCH -> REFRESH

    REFRESH -> CHK_KEY

    /* Windows-only resize path */
    CHK_KEY -> WIN_RESIZE [label="#ifdef _WIN32\nresize event"]
    WIN_RESIZE -> CHK_KEY

    CHK_KEY -> CHK_LOOP [label="key read"]

    CHK_LOOP -> COPY_OUT [label="yes (Enter)"]
    CHK_LOOP -> CONFIRM [style=invis]
    COPY_OUT -> CONFIRM
    CONFIRM -> END_OK

    CHK_LOOP -> CHK_KEY2 [label="no"]

    node [shape=diamond fillcolor="#fff7d6" fontcolor=black color="#b07800"]
    CHK_KEY2 [label="Which key?"]

    CHK_KEY2 -> RESIZE_WIN [label="KEY_RESIZE"]
    CHK_KEY2 -> EOT_ACT [label="ETX / EOT"]
    CHK_KEY2 -> NAV_UP [label="UP / LEFT"]
    CHK_KEY2 -> NAV_DOWN [label="DOWN / RIGHT"]
    CHK_KEY2 -> CHK_BUFLEN [label="BACKSPACE"]
    CHK_KEY2 -> CHK_PRINT [label="other"]

    RESIZE_WIN -> LOOP_INIT
    EOT_ACT -> END_ERR2
    NAV_UP -> LOOP_INIT
    NAV_DOWN -> LOOP_INIT

    CHK_BUFLEN -> SEARCH_POP [label="yes"]
    CHK_BUFLEN -> LOOP_INIT [label="no (ignore)"]
    SEARCH_POP -> LOOP_INIT

    CHK_PRINT -> SEARCH_APP [label="yes"]
    CHK_PRINT -> LOOP_INIT [label="no (ignore)"]
    SEARCH_APP -> LOOP_INIT
}
```
Scrolling is done by simply centering the current selection, and printing the other options using the current selection's index as the offset.

\ref user_select_menu() is a wrapper around simple_select(), it finds the users with the matching usertypes, passes it to simple_select, then returns the values from simple_select.

The fuzzy search is based on the Levenshtein distance algorithm. we find the Levenshtein distance between the user's query and every option on each keypress, then set the selection to the option with the lowest distance.

https://en.wikipedia.org/wiki/Levenshtein_distance

\section command_select_design Command Select

\ref command_select()

```dot
digraph command_select {
    graph [
        rankdir=TB
        splines=polyline
        nodesep=0.5
        ranksep=0.55
        fontname="Helvetica"
        bgcolor="white"
    ]
    node [
        fontname="Helvetica"
        fontsize=11
        style=filled
        fillcolor="#f0f4ff"
        color="#3a5aad"
        penwidth=1.5
    ]
    edge [
        fontname="Helvetica"
        fontsize=10
        color="#444444"
        penwidth=1.2
    ]

    /* ── Terminals ── */
    node [shape=oval fillcolor="#1a2a5e" fontcolor=white color="#1a2a5e"]
    START       [label="START"]
    END_ERR     [label="endwin()\nexit(1)\n(no commands)"]
    END_ETX     [label="endwin()\nRETURN  (ETX/Ctrl-C)"]
    END_EOT     [label="endwin()\nRETURN  (EOT/Ctrl-D)"]

    /* ── Process boxes ── */
    node [shape=box fillcolor="#dce8ff" fontcolor=black color="#3a5aad"]
    COPY_LIST [label="Walk linked list → cmd_list[]\ncmd_list_n = count"]
    INIT      [label="ncurses_init()\ninit_color(10, purple)\ninit_pair(2, WHITE, 10)\nclear() · getmaxyx(stdscr)"]
    LOOP_TOP  [label="── loop top ──"]
    W32_APPLY [label="y_max, x_max = winsize\nresize_term() · clear()"]
    REFRESH   [label="refresh()  ← resumes ncurses after endwin()"]
    MK_WINS   [label="left_win  = newwin(y_max, x_max/2, 0, 0)\nright_win = newwin(y_max, x_max/2, 0, x_max/2+1)\nbox(left_win) · keypad(left_win, TRUE)"]
    DRAW_L    [label="Draw left_win:\n  header: username + utype\n  for each cmd: skip if covers header\n  highlight selection with A_REVERSE\n  wbkgd(COLOR_PAIR(2))"]
    DRAW_R    [label="Draw right_win:\n  Permission label\n  Argument types (loop over n_of_args)\n  Description"]
    WREFRESH  [label="wrefresh(left_win)\nwrefresh(right_win)"]
    GETCH     [label="ch = getch()"]
    NAV_UP    [label="selection = (selection - 1 + N) % N"]
    NAV_DOWN  [label="selection = (selection + 1) % N"]
    RESIZE_A  [label="getmaxyx(stdscr) · clear()"]
    ENDWIN_E  [label="endwin()"]
    PERM_DENY [label="printf: Permission denied\ngetchar()"]
    ALLOC     [label="args = malloc(sizeof(Value) * n)"]
    SCANF_L   [label="for each arg:\n  printf prompt\n  scanf per type\n  string_trim (strings only)\nwipe_stdin()"]
    CALL_A    [label="retval = call(cmd, args, n, utype)\nprint_Value(retval)\nfree(args)"]
    PAUSE_A   [label="raw() + getch()  [Linux]\nor getchar()  [Windows]\ngetmaxyx(stdscr) · clear()"]
    CALL_0    [label="retval = call(cmd, varray_username, 0, utype)\nprint_Value(retval)"]
    PAUSE_0   [label="raw() + getch()  [Linux]\nor getchar()  [Windows]\ngetmaxyx(stdscr)\n⚠ no clear() here"]
    DELWINS   [label="delwin(right_win)\ndelwin(left_win)"]

    /* ── Decisions ── */
    node [shape=diamond fillcolor="#fff7d6" fontcolor=black color="#b07800"]
    CHK_N    [label="cmd_list_n == 0?"]
    CHK_W32  [label="#ifdef _WIN32\nresize event?"]
    CHK_KEY  [label="Which key?"]
    CHK_PERM [label="utype ==\ncmd.permission?"]
    CHK_ARGS [label="n_of_args > 0?"]

    /* ── Edges ── */
    START     -> COPY_LIST
    COPY_LIST -> CHK_N
    CHK_N     -> END_ERR   [label="yes"]
    CHK_N     -> INIT      [label="no"]
    INIT      -> LOOP_TOP

    LOOP_TOP  -> CHK_W32
    CHK_W32   -> W32_APPLY [label="yes"]
    W32_APPLY -> REFRESH
    CHK_W32   -> REFRESH   [label="no / not Win32"]

    REFRESH  -> MK_WINS
    MK_WINS  -> DRAW_L
    DRAW_L   -> DRAW_R
    DRAW_R   -> WREFRESH
    WREFRESH -> GETCH
    GETCH    -> CHK_KEY

    CHK_KEY -> END_ETX   [label="ETX"]
    CHK_KEY -> END_EOT   [label="EOT"]
    CHK_KEY -> NAV_UP    [label="UP / LEFT"]
    CHK_KEY -> NAV_DOWN  [label="DOWN / RIGHT"]
    CHK_KEY -> RESIZE_A  [label="KEY_RESIZE"]
    CHK_KEY -> ENDWIN_E  [label="Enter (\\n)"]
    CHK_KEY -> DELWINS   [label="other"]

    NAV_UP   -> DELWINS
    NAV_DOWN -> DELWINS
    RESIZE_A -> DELWINS

    ENDWIN_E -> CHK_PERM
    CHK_PERM -> PERM_DENY [label="denied"]
    PERM_DENY -> DELWINS

    CHK_PERM -> CHK_ARGS [label="allowed"]

    CHK_ARGS -> ALLOC    [label="yes (n > 0)"]
    ALLOC    -> SCANF_L
    SCANF_L  -> CALL_A
    CALL_A   -> PAUSE_A
    PAUSE_A  -> DELWINS

    CHK_ARGS -> CALL_0   [label="no (n == 0)"]
    CALL_0   -> PAUSE_0
    PAUSE_0  -> DELWINS

    DELWINS  -> LOOP_TOP [label="while(1)"]
}
```


\section cfg_editor Config editor
\ref config_editor() is a function that provides a TUI for editing `key: value` or `key=value` files.
\n It shows and scrolls the keys on the left side, while showing the value on the right side. 
\n When pressing enter, the user can switch between EDIT mode and SELECT mode.
\n During EDIT mode, the user can use their keyboard to edit the value.


\image html config_editor.png "SELECT mode"
\image latex config_editor.png "SELECT mode"
\n \n
\image html config_editor_edit.png "EDIT mode, with blinking cursor"
\image latex config_editor_edit.png "EDIT mode, with blinking cursor"


Also you can pass a string array to prevent users from changing some fields

\image html config_editor_no.png ""
\image latex config_editor_no.png ""



\ref KEY_RESIZE is not handled after the first switch case, so it effectively restarts the loop and refreshes the menu

Flowchart for \ref config_editor
```dot
digraph ConfigEditor {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    Start [shape=ellipse, style=filled, fillcolor=lightblue, label="Start config_editor()"];
    InitNcurses [label="Initialize ncurses\n& clear screen"];
    LoopStart [shape=point];
    
    CheckEditMode [shape=diamond, label="edit_mode == 1?"];
    
    DrawEditMode [label="Draw EDIT MODE\nDraw edit_buffer in place of value\nMove cursor to value"];
    DrawSelectMode [label="Draw SELECT MODE\nDraw left_win (config keys)\nDraw right_win (values or warnings)"];
    
    GetCh [label="ch = getch()"];
    
    GlobalKeys [shape=diamond, label="Key?"];
    HandleResize [label="Get terminal size\n(KEY_RESIZE)"];
    HandleExit [shape=ellipse, style=filled, fillcolor=lightpink, label="Exit without saving\n(KEY_ETX, KEY_EOT)\nreturn -1"];
    
    CheckModeForKeys [shape=diamond, label="edit_mode == 1?"];
    
    // Edit Mode Keys
    EditKeySwitch [shape=diamond, label="Key?"];
    MoveCursor [label="Move Cursor\n(HOME, END, LEFT, RIGHT)"];
    ModifyBuffer [label="Modify Buffer\n(BACKSPACE, DEL, ASCII chars)"];
    ConfirmEdit [label="Save edit_buffer to config entry\nedit_mode = 0\n(Enter)"];
    
    // Select Mode Keys
    SelectKeySwitch [shape=diamond, label="Key?"];
    SaveAndExit [shape=ellipse, style=filled, fillcolor=lightgreen, label="Save config to file\nreturn 0\n(CTRL_W)"];
    ChangeSelection [label="Change Selection\n(UP, DOWN, LEFT, RIGHT)"];
    TryEdit [shape=diamond, label="Is key in disallow list?\n(Enter)"];
    EnterEditMode [label="edit_mode = 1\nCopy value to edit_buffer"];
    SetDisallowWarning [label="allow = 0\n(Show warning next loop)"];
    
    CleanupWins [label="Delete windows if they exist\n(left_win, right_win)"];

    // Flow
    Start -> InitNcurses -> LoopStart;
    LoopStart -> CheckEditMode;
    
    CheckEditMode -> DrawEditMode [label="Yes"];
    CheckEditMode -> DrawSelectMode [label="No"];
    
    DrawEditMode -> GetCh;
    DrawSelectMode -> GetCh;
    
    GetCh -> GlobalKeys;
    
    GlobalKeys -> HandleResize [label="KEY_RESIZE"];
    HandleResize -> CheckModeForKeys;
    
    GlobalKeys -> HandleExit [label="EOF / SIGINT"];
    
    GlobalKeys -> CheckModeForKeys [label="Other"];
    
    CheckModeForKeys -> EditKeySwitch [label="Yes"];
    CheckModeForKeys -> SelectKeySwitch [label="No"];
    
    EditKeySwitch -> MoveCursor [label="Nav Keys"];
    EditKeySwitch -> ModifyBuffer [label="Typing / Del"];
    EditKeySwitch -> ConfirmEdit [label="Enter"];
    EditKeySwitch -> CleanupWins [label="Other"];
    
    MoveCursor -> CleanupWins;
    ModifyBuffer -> CleanupWins;
    ConfirmEdit -> CleanupWins;
    
    SelectKeySwitch -> SaveAndExit [label="CTRL+W"];
    SelectKeySwitch -> ChangeSelection [label="Nav Keys"];
    SelectKeySwitch -> TryEdit [label="Enter"];
    SelectKeySwitch -> CleanupWins [label="Other"];
    
    ChangeSelection -> CleanupWins;
    
    TryEdit -> EnterEditMode [label="No"];
    TryEdit -> SetDisallowWarning [label="Yes"];
    
    EnterEditMode -> CleanupWins;
    SetDisallowWarning -> CleanupWins;
    
    CleanupWins -> LoopStart;
}

TODO: explain insert mode during edit mode
```

\section time_pick_design Time picker 

```dot
digraph time_picker {
    graph [
        rankdir=TB
        splines=polyline
        nodesep=0.5
        ranksep=0.55
        fontname="Helvetica"
        bgcolor="white"
    ]
    node [
        fontname="Helvetica"
        fontsize=11
        style=filled
        fillcolor="#f0f4ff"
        color="#3a5aad"
        penwidth=1.5
    ]
    edge [
        fontname="Helvetica"
        fontsize=10
        color="#444444"
        penwidth=1.2
    ]

    /* ── Terminals ── */
    node [shape=oval fillcolor="#1a2a5e" fontcolor=white color="#1a2a5e"]
    START       [label="START"]
    END_ERR     [label="endwin()\nRETURN (time_t)-1\n(ETX/EOT)"]
    END_OK      [label="endwin()\nRETURN time\n(Enter)"]

    /* ── Process boxes ── */
    node [shape=box fillcolor="#dce8ff" fontcolor=black color="#3a5aad"]
    INIT     [label="ts = localtime(&time)\n"]
    NCURSES  [label="ncurses_init()\ninit_pair(2, fg, bg)"]
    PRE_TEXT [label="mvprintw(0,0, pretext)\n"]
    LOOP_TOP [label="── loop top ──"]
    SET_RS1  [label="switch(select) → strcpy(range_s, ...)\n(Year / Month / Day / Hour / Minute / Second)"]
    MKTIME   [label="time = mktime(ts)\n"]
    DRAW     [label="mvprintw(1): readable_time\nmvprintw(2): range_s  [A_REVERSE]\nmvprintw(3): (unsigned long)time\n⚠ truncates on 64-bit"]
    REFRESH  [label="refresh()"]
    GETCH    [label="ch = getch()"]
    MV_UP    [label="change = 1"]
    MV_DN    [label="change = -1"]
    SEL_L    [label="select = (select - 1 + 6) % 6\nchange = 0"]
    SEL_R    [label="select = (select + 1) % 6\nchange = 0"]
    CLR_CH   [label="change = 0"]
    APPLY    [label="ts->tm_[field] += change\n"]

    /* ── Decisions ── */
    node [shape=diamond fillcolor="#fff7d6" fontcolor=black color="#b07800"]
    CHK_KEY [label="Which key?"]

    /* ── Edges ── */
    START    -> INIT
    INIT     -> NCURSES
    NCURSES  -> PRE_TEXT
    PRE_TEXT -> LOOP_TOP

    LOOP_TOP -> SET_RS1
    SET_RS1  -> MKTIME
    MKTIME   -> DRAW
    DRAW     -> REFRESH
    REFRESH  -> GETCH
    GETCH    -> CHK_KEY

    CHK_KEY -> END_ERR [label="ETX / EOT"]
    CHK_KEY -> MV_UP   [label="UP"]
    CHK_KEY -> MV_DN   [label="DOWN"]
    CHK_KEY -> SEL_L   [label="LEFT"]
    CHK_KEY -> SEL_R   [label="RIGHT"]
    CHK_KEY -> END_OK  [label="Enter (\\n)\n"]
    CHK_KEY -> CLR_CH  [label="KEY_RESIZE\nor default"]

    MV_UP -> APPLY
    MV_DN -> APPLY
    SEL_L -> APPLY
    SEL_R -> APPLY
    CLR_CH -> APPLY

}
```

\section text_edit_design Text editor
We decided to write our own text editor to reduce the amount of dependencies HMS uses. 

We *could* just use nano, vim, or notepad, but those are extra dependencies and might break the "No GUI" requirement. 

It serves as a terminal pager (although it loads the entire file) and is a very primitive text editor. 

Like nano, the functions loads the file by parsing each line as a node in a doubly linked list, which made insertions much faster (O(1)) and more practical to write. If we stored the lines in a `char **` (array of string pointers), insertions and removals are O(n) and are more annoying to manage.

see:\n 
- \ref file_to_LL_lines()
- \ref text_editor()

Features: 
- readonly
- vertical and horizontal scrolling
- line numbers
- dynamic memory allocation
- splitting newlines

While typing, text_editor() will automatically allocate more memory the more you type.

```C
	if((ch >= 32 && ch <= 126) && !readonly) /* printable ASCII */
	{
		CurrentLine->str_n++;
		if(CurrentLine->str_n % LINE_REALLOC_LIMIT == 0)
			realloc_string(&CurrentLine->str, CurrentLine->str_n + LINE_REALLOC_LIMIT);
		string_insert(CurrentLine->str, ch, true_x);
...
```

When the user creates a newline and splits a line, or merges a line from backspace, text_editor must allocate the next multiple of 


```dot
digraph TextEditor {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    Start [shape=ellipse, style=filled, fillcolor=lightblue, label="Start text_editor()"];
    Init [label="file_to_LL_lines()\nInit ncurses"];
    LoopStart [shape=point];
    
    Render [label="Calculate offsets\nPrint visible lines\nPrint status bar\nMove cursor"];
    GetCh [label="ch = getch()"];
    
    KeySwitch [shape=diamond, label="Key?"];
    
    Nav [label="Update cur_x, cur_y,\nx_offset, y_offset\nMove CurrentLine pointer"];
    
    InsertChar [label="Insert char into CurrentLine.str\nRealloc if needed\nUpdate cur_x"];
    
    EnterKey [label="Insert New Line Node\nSplit string & realloc\n(See Node Insertion)"];
    
    BackspaceDel [label="Remove char\nOR Delete Node if at line start\n(See Node Deletion)"];
    
    Resize [label="Update terminal size\nClear screen"];
    
    Exit [shape=ellipse, style=filled, fillcolor=lightpink, label="Discard changes & Exit"];
    Save [shape=ellipse, style=filled, fillcolor=lightgreen, label="Save & Exit"];
    NextLoop [shape=ellipse, style=filled, fillcolor=lightyellow, label="Next loop"];

    Start -> Init -> LoopStart;
    LoopStart -> Render -> GetCh -> KeySwitch;
    
    KeySwitch -> Nav [label="Arrow keys,\nHOME, END"];
    KeySwitch -> InsertChar [label="Printable ASCII / TAB"];
    KeySwitch -> EnterKey [label="Enter (\\n)"];
    KeySwitch -> BackspaceDel [label="Backspace / Delete"];
    KeySwitch -> Resize [label="KEY_RESIZE"];
    KeySwitch -> Exit [label="SIGINT / EOF"];
    KeySwitch -> Save [label="CTRL+W"];
    
    Nav -> NextLoop;
    InsertChar -> NextLoop;
    EnterKey -> NextLoop;
    BackspaceDel -> NextLoop;
    Resize -> NextLoop;
    
    NextLoop -> LoopStart;
}
```

When you press enter in a text editor, it creates a new line. Naively you'd just insert a `\n` in the string. 

In our case, we need to do node insertion, and manually allocate memory when splitting a line. When a line is split between the cursor, the memory size of the previous line is the length the prefix, while the next line is the length of the suffix.

Comment from calculating the size of the new line, that explains why we need to find the new memory size: 
```C
/* The next realloc wont happen until str_n is a multiple of LINE_REALLOC_LIMIT,
 * So, naively allocating str_n - true_x will cause a buffer overflow.
 * For example: (if the limit is 10)
	* "Hello, world\0" 13 bytes
	* 				^
	* 
	* User types extra three letters
	* 
	* "Helloooo, world\0" 16 bytes, but we did not trigger realloc
	* 				^
	*		Buffer overflow!!
 */
```

```dot
digraph MemAllocInsertion {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    Start [shape=ellipse, style=filled, fillcolor=lightblue, label="Enter key pressed\n(Split Line)"];
    CheckPos [shape=diamond, label="Cursor at end of line?\n(true_x == CurrentLine.str_n)"];
    
    AllocEmpty [label="New.str = malloc(LIMIT)\nNew.str_n = 0\nNew.str[0] = '\\0'"];
    
    CalcSize [label="suffix_len = CurrentLine.str_n - true_x\nsize = ((suffix_len / LIMIT) + 1) * LIMIT"];
    AllocSuffix [label="New.str = malloc(size)\nstrcpy(New.str, &CurrentLine.str[true_x])\nNew.str_n = suffix_len"];
    TruncatePrefix [label="CurrentLine.str[true_x] = '\\0'\nCurrentLine.str_n = true_x"];
    
    Start -> CheckPos;
    CheckPos -> AllocEmpty [label="Yes"];
    CheckPos -> CalcSize [label="No"];
    CalcSize -> AllocSuffix -> TruncatePrefix;
}
```

```dot
digraph NodeInsertion {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    subgraph cluster_step1 {
        label="Step 1: Initial State";
        color=grey;
        C1 [label="CurrentLine"];
        N1 [label="NextNode"];
        
        C1 -> N1 [label="next"];
        N1 -> C1 [label="prev"];
        {rank=same; C1; N1}
    }
    
    subgraph cluster_step2 {
        label="Step 2: New.next = CurrentLine.next";
        color=grey;
        C2 [label="CurrentLine"];
        New2 [label="New Node", color=blue, fontcolor=blue];
        N2 [label="NextNode"];
        
        C2 -> N2 [label="next, prev", dir="both"];
        New2 -> N2 [label="next", color=blue, fontcolor=blue];
        {rank=same; C2; New2; N2}
    }

    subgraph cluster_step3 {
        label="Step 3: CurrentLine.next.prev = New";
        color=grey;
        C3 [label="CurrentLine"];
        New3 [label="New Node", color=blue, fontcolor=blue];
        N3 [label="NextNode"];
        
        C3 -> N3 [label="next", style=dashed, color=grey];
        N3 -> New3 [label="prev", color=blue, fontcolor=blue];
        New3 -> N3 [label="next"];
        {rank=same; C3; New3; N3}
    }

    subgraph cluster_step4 {
        label="Step 4: CurrentLine.next = New";
        color=grey;
        C4 [label="CurrentLine"];
        New4 [label="New Node", color=blue, fontcolor=blue];
        N4 [label="NextNode"];
        
        C4 -> New4 [label="next", color=blue, fontcolor=blue];
        N4 -> New4 [label="prev"];
        New4 -> N4 [label="next"];
        {rank=same; C4; New4; N4}
    }

    subgraph cluster_step5 {
        label="Step 5: New.prev = CurrentLine";
        color=grey;
        C5 [label="CurrentLine"];
        New5 [label="New Node", color=blue, fontcolor=blue];
        N5 [label="NextNode"];
        
        C5 -> New5 [label="next"];
        New5 -> C5 [label="prev", color=blue, fontcolor=blue];
        N5 -> New5 [label="prev"];
        New5 -> N5 [label="next"];
        {rank=same; C5; New5; N5}
    }

    // Force vertical ordering
    C1 -> C2 -> C3 -> C4 -> C5 [style=invis, weight=100];
}
```

Inversely, removing a line would just be removing a `\n`

When backspacing at the start of the line, The current line is concatenated to the previous line, then the current line is deleted. 
\n We need to do node deletion, and calculate the new size.

```dot
digraph MemAllocDeletion {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    Start [shape=ellipse, style=filled, fillcolor=lightblue, label="Backspace at start of line\n(Merge Lines)"];
    CheckLen [shape=diamond, label="(Prev.str_n + CurrentLine.str_n) > 0?"];
    
    CalcSize [label="total_len = Prev.str_n + CurrentLine.str_n\nsize = ((total_len + 1) / LIMIT + 1) * LIMIT"];
    Realloc [label="realloc_string(&Prev.str, size)"];
    Concat [label="strcat(Prev.str, CurrentLine.str)\nPrev.str_n = total_len"];
    
    NoAlloc [label="No reallocation needed\n(Both lines empty)"];

    Start -> CheckLen;
    CheckLen -> CalcSize [label="Yes"];
    CheckLen -> NoAlloc [label="No"];
    CalcSize -> Realloc -> Concat;
}
```

```dot
digraph NodeDeletion {
    rankdir=TB;
    node [shape=box, style=rounded, fontname="sans-serif"];
    edge [fontname="sans-serif", fontsize=10];

    subgraph cluster_step1 {
        label="Step 1: Initial State";
        color=grey;
        P1 [label="PrevNode"];
        C1 [label="node", color=red, fontcolor=red];
        N1 [label="NextNode"];
        
        P1 -> C1 [label="next"];
        C1 -> P1 [label="prev"];
        C1 -> N1 [label="next"];
        N1 -> C1 [label="prev"];
        {rank=same; P1; C1; N1}
    }
    
    subgraph cluster_step2 {
        label="Step 2: node.prev.next = node.next";
        color=grey;
        P2 [label="PrevNode"];
        C2 [label="node", color=red, fontcolor=red];
        N2 [label="NextNode"];
        
        P2 -> C2 [label="next", style=dashed, color=grey];
        P2 -> N2 [label="next", color=blue, fontcolor=blue];
        C2 -> P2 [label="prev"];
        C2 -> N2 [label="next"];
        N2 -> C2 [label="prev"];
        {rank=same; P2; C2; N2}
    }

    subgraph cluster_step3 {
        label="Step 3: node.next.prev = node.prev";
        color=grey;
        P3 [label="PrevNode"];
        C3 [label="node", color=red, fontcolor=red];
        N3 [label="NextNode"];
        
        P3 -> N3 [label="next"];
        C3 -> P3 [label="prev"];
        C3 -> N3 [label="next"];
        N3 -> C3 [label="prev", style=dashed, color=grey];
        N3 -> P3 [label="prev", color=blue, fontcolor=blue];
        {rank=same; P3; C3; N3}
    }

    subgraph cluster_step4 {
        label="Step 4: Free node";
        color=grey;
        P4 [label="PrevNode"];
        N4 [label="NextNode"];
        
        P4 -> N4 [label="next"];
        N4 -> P4 [label="prev"];
        {rank=same; P4; N4}
    }

    // Force vertical ordering
    P1 -> P2 -> P3 -> P4 [style=invis, weight=100];
}
```
