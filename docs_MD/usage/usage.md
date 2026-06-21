\page usage How to use HMS

TIP: Do CTRL+C (sigint) to exit the program or cancel an operation while using a TUI 

The binary only requires the DB folder and credentials file, and you can name the binary whatever you want

**It's very important that the binary is in the same dir as your DB file!**

\n This will show the menu first:
```bash
./main.out
```


\n To go straight to login or register:
```bash
./main.out login
```
```bash
./main.out register
```


\n To login into an account
```bash
./main.out login [username]
```

\section simple_tui Simple Select
Press arrow keys to change selection, and enter to confirm selection. CTRL+C to cancel

Your selection will be centered and highlighted

Type and backspace to search for options. It does fuzzy search on your query and every option, then finds the closest option for your query.

Partial matching won't always work, but it is guaranteed that it will find exact matches

\image html simpleselect.png "Simple select being used for search and view in director's 'View payment history' command"
\image latex simpleselect.png "Simple select being used for search and view in director's 'View payment history' command"

\n

\image html simpleselect_query.png "Simple select's fuzzy search at work, moving selection to partial match"
\image latex simpleselect_query.png "Simple select's fuzzy search at work, moving selection to partial match"

\section config_edit_usage Config Editor
Up and down arrow keys to select fields.

\n When pressing enter, the user can switch between EDIT mode and SELECT mode.
\n During EDIT mode, the user can use their keyboard to edit the value.


\image html config_editor.png "SELECT mode"
\image latex config_editor.png "SELECT mode"
\n \n
\image html config_editor_edit.png "EDIT mode, with blinking cursor"
\image latex config_editor_edit.png "EDIT mode, with blinking cursor"


You will not be able to edit some fields

\image html config_editor_no.png ""
\image latex config_editor_no.png ""





\section command_tui  Command menu
The left panel will show commands available to your user type, the right panel will show the command's arguments and description.

Press arrow keys to change selection, and enter to confirm selection.

\image html command_menu.png "Admins' 'create account' command"
\image latex command_menu.png "Admins' 'create account' command"

If your command has arguments, you will be prompted to enter the arguments, otherwise the command will run immediately.

You will be dropped back to normal terminal for scanf. 

\image html command_menu_done.png "The execution of the 'create account' command"
\image latex command_menu_done.png "The execution of the 'create account' command"

After the command execution is done, you will be given an opportunity to view the command's output, then press any key to go back to command menu (or enter, if you're on windows.)

The command menu doesn't clear the terminal every time, so you may see output from previous commands or other things, like "Password hash matched" when you log in


\section booking_tui Booking menu
Up and down arrow keys to select slot 

When pressing enter on a slot, you might be given options for what to do with the slots, with simple select

As a patient, you will not be able to see the information of slot that other patient selected.

\image html booking_view.png
\image latex booking_view.png

\section time_tui Time selection
Left and right arrow keys to change the time quantity, then up and down arrow keys to modify the time with that quantity. Press enter to confirm.

\image html time_select.png "Director's sales report command, which prompts the user to select a time range."
\image latex time_select.png "Director's sales report command, which prompts the user to select a time range."