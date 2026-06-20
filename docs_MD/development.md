\page dev Developing the codebase
We have provided debug tasks for Visual Studio Code and Zed editor, which are located in .zed and .vscode

clangd and CodeLLDB are automatically installed by Zed editor, tested on Arch and macOS

If you are using Visual Studio Code, you need to install the extensions:
- clangd (For strict ANSI C syntax highlighting)
- C/C++ Extension Pack
- CodeLLDB or LLDB DAP

You also need to have `gdb` installed

When using makefile to compile, you will see a warning for `crypt_gensalt_ra` from `crypt_blowfish`. You can safely ignore this because we aren't using that function; you can edit the source to comment out the function

\section add_commands Adding commands to the program 

At the first half of main in main.c, you will see cmd_list, which is what the menu after login shows.
```C
	static ValueType makeuser_args[] = {TYPE_STRING, TYPE_STRING};

	/* COMMAND(function name, alias, permission, argument types) */
	const command cmd_list[] = {
        COMMAND_NOARGS(doctor_booking_frontend, "Doctor Booking", "Doctor", "Booking options for doctors"),
        COMMAND_NOARGS(patient_booking_frontend, "Patient Booking", "Patient", "Booking options for patients"),

        COMMAND_NOARGS(admin_view_cfg, "View users profile", "Admin", ""),
        COMMAND(make_user, "Create account", "Admin", makeuser_args, "Create accounts for patients, doctors, etc\n\n First arg is username,\n second arg is usertype (cAsE sEnSiTiVe)"),
        COMMAND_NOARGS(edit_profile_self_admin, "Edit your profile", "Admin", ""),



        COMMAND_NOARGS(nurse_view_booking, "View doctor's appointments", "Nurse", ""),
	};
```
The data structs used here are:
- ValueType (which is just used to make enum arrays)
- Value (contains a union for int, float, and strings, and a tag to use which field. Like a python variable that can store arbitrary datatypes)
- \ref command (contains a type-erased function pointer, ValueType array (enum array) to tag the datatype of each param, and a string to describe who can use the command)

\ref COMMAND and \ref COMMAND_NOARGS are macros that will simplify populating the command array.

The return type must be Value and the param must be a Value array or void. Or else you will get undefined behaviour. 

For example, to make a nurse command that takes an int, string, and float, and prints everything out:
```C 
void pront(Value* list)
{
	int i;
	for(i=0; i<3; i++)
	{
		switch (list[i].type)
		{
			case TYPE_INT:    	printf("int: %d\n",   list[i].data.as_int);    break;
			case TYPE_FLOAT:  	printf("float: %f\n", list[i].data.as_float);  break;
			case TYPE_STRING: 	printf("str: %s\n",   list[i].data.as_string); break;
			case TYPE_BOOL:			printf("bool: %d\n",  list[i].data.as_bool);   break;
			case TYPE_VOID:			printf("void \n");														 break;
		}
	}
}
```
Put pront in a header file and include in main.c

Above cmd_list:
```C
static ValueType pront_args[] = {TYPE_INT, TYPE_STRING, TYPE_FLOAT};
```

Then in cmd_list, use the \ref COMMAND macro:
```C
COMMAND(pront, "Pront", "Nurse", pront_args, "Print 3 things"),
```
The first string is what is shown in the menu, the second is who is allowed to use the command, and the third is the description \n \ref command_select will automatically prompt the user for params, and \ref call will check each type and finally call the function with the function pointer.

The pattern of casting a function pointer to a type erased pointer, labeling it in a struct, and calling the function with the pointer later is called "manual vtable dispatch".

Places where you'll see this are CPython, the Linux kernel, and the C stdlib (For example qsort, which takes `int (*compar)(const void *, const void *)` as a param).

\section db_dev Modifying the database
\ref db

Currently, the paths for the database are hardcoded, so you may need to define headers for paths in \ref globals.h .

If you want to make changes to the database structure, modify file_struct.yaml.txt and test with \ref regen_file_structure.c

```bash 
		gcc -o regen_file.out regen_file_structure.c utils/utils.c utils/string_utils.c -g -ansi
		./regen_file.out
```