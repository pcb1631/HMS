\page tui Text User Interfaces
We make use of the ncurses library to create TUIs, in contrast to our previous python project where we had to manually deal with ANSI escape keys.
\n This doesn't mean ncurses will abstract everything, it still requires knowledge of the terminal and how ncurses works. e.g. clear() triggers a full rerender to the next refresh() call, and refresh() restores previous settings (raw mode, noecho, keypad, etc)

The proper pattern is to use an init function (ncurses handles state externally, so using a function is fine), call refresh() every loop, and call clear() if doing anything that leaves characters in ncurses's buffer (i.e. backspacing in text editor)


\section cfg_editor Config editor
TODO: we can be vague about the left side because im going to write about others above

\ref config_editor is a function that provides a TUI for editing `key: value` or `key=value` files.
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
