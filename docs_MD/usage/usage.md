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

\section simple_tui Simple menu
Press arrow keys to change selection, and enter to confirm selection. CTRL+C to cancel

Type and backspace to search for options. It does fuzzy search on your query and every option, then finds the closest option for your query.


\section command_tui  Command menu

\section booking_tui Booking menu

\section time_tui Time selection
Left and right arrow keys to change the time range, then up and down arrow keys to modify the time with that range. Press enter to confirm.

\section login Login/Register
