\page cmd_dispatch Command dispatch

Typically, a main menu for this kind of program would be prompting an input, and using a switch case to run the correct function.

In our case, we store an array of type-erased function pointers, prompt for an input, and call the function pointer in the array.

Calling, and fetching the arguments from a reference to arbitrary functions is very simple in Python. However in C, we need to cast the function pointer to one with a void signature (`void (*)(void)`). That means it is imperative that we remember the return type and argument types to avoid undefined behaviour.

For simplicity, we will only make functions in the command list with return type Value, and the arguments a Value array or void. 

\section cmd_value The Value struct
\ref Value
\n The aim is to make something like a python variable; it can hold arbitrary data

It contains a union with int, float, and char. But we indicate which field to use with a label, the ValueType enum.

The helper functions are make_int(), make_float(), make_string().

Example of how a Value variable is init:
```C
Value make_int(int x)
{
	Value v;
	v.type = TYPE_INT;
	v.data.as_int = x;
	return v;
}
```

An example of how a Value variable is accessed:
```C
void print_Value(Value x)
{
	switch (x.type)
	{
		case TYPE_INT:
		printf("%d", x.data.as_int);
		break;
		
		case TYPE_BOOL:
		printf("%d", x.data.as_int);
		break;
		
		case TYPE_STRING:
		printf("%s", x.data.as_string);
		break;

		case TYPE_FLOAT:
		printf("%f", x.data.as_float);
		break;
		
		case TYPE_VOID:
		break;

		default:
		return;
	}
}
```


\section cmd_commandstruct The command struct, and calling a command
\ref command

Since we know that the command functions will only return Value, we just need to store the type-erased pointer, a ValueType enum array to remember the type of every element in a Value array, and the length of the array to call a command function (`(Value(*)(Value *))`).

But we can indicate that a function is `Value(*)(void)` by setting n of args to 0.

To make sure Patients don't run dangerous admin commands, the struct also stores a string to match the current user's usertype.

```C
Value call(command f, Value * args, int n_of_args, char * perm)
{
	int i;
	Value err = make_int(0);

	if(strcmp(perm, f.permission) != 0)
	{
		fprintf(stderr, "Permission denied (current user perm: %s, command perm: %s)\n", perm, f.permission);
		return err;
	}

	if(f.n_of_args == 0)
		return ((Value (*)(void))f.ptr)();

	if(n_of_args != f.n_of_args && n_of_args != -1)
	{
		fprintf(stderr, "Command %s takes %d args, but was given %d\n", f.alias, f.n_of_args, n_of_args);
		return err;
	}

	/* Check types */
	for(i=0; i<n_of_args; i++)
	{
		if(f.arg_type[i] == TYPE_VOID)
			break;
		if(f.arg_type[i] != args[i].type)
		{
			fprintf(stderr, "argument %d type mismatch for command %s\n", i, f.alias);
			return err;
		}
	}

	return ((Value (*)(Value *))f.ptr)(args);
}
```

The purpose of commands returning call is custom exit messages.

\section cmd_list The command list
Before the program runs \ref command_select(), it constructs a singly linked LL where every command in it matches the current user's permissions, then HEAD is passed to \ref command_select(). 

To make it easier to add commands to the array, there are macros \ref COMMAND() and \ref COMMAND_NOARGS()

```C
	static ValueType makeuser_args[] = {TYPE_STRING, TYPE_STRING};
	static ValueType patient_payment_args[] = {TYPE_FLOAT, TYPE_STRING};

	/* COMMAND(function name, alias, permission, argument types) */
	const command cmd_list[] = {

		COMMAND_NOARGS(doctor_booking_frontend, "Doctor Booking", "Doctor", "Booking options for doctors"),
		COMMAND_NOARGS(doctor_diagnosis, "Write diagnosis", "Doctor", "Create a diagnosis report for a patient"),
		COMMAND_NOARGS(view_diagnosis_self_doc, "View diagnosis", "Doctor", "View diagnosis reports for a patient you wrote yrself"),
		COMMAND_NOARGS(doctor_view_patients_profile, "View patient's profile", "Doctor", ""),
		COMMAND_NOARGS(edit_patient_desc, "Edit patient's report", "Doctor", ""),
		
		COMMAND_NOARGS(patient_booking_frontend, "Patient Booking", "Patient", "Booking options for patients"),
		COMMAND_NOARGS(edit_profile_self_patient, "Edit your profile", "Patient", ""),
		COMMAND_NOARGS(create_complaint, "Create complaint", "Patient", "spill the tea sis"),
		COMMAND(perform_payment, "Pay", "Patient", patient_payment_args, "Pay your bills.\nFirst arg is amount of money, second is description"),
		COMMAND_NOARGS(view_payment_history_patient, "View your payment history", "Patient", ""),
		COMMAND_NOARGS(view_complaints_self, "Search and edit your complaints", "Patient", ""),
		
		COMMAND_NOARGS(admin_view_cfg, "View users profile", "Admin", ""),
		COMMAND(make_user, "Create account", "Admin", makeuser_args, "Create accounts for patients, doctors, etc\n\n First arg is username,\n second arg is usertype (cAsE sEnSiTiVe)"),
		COMMAND_NOARGS(edit_profile_self_admin, "Edit your profile", "Admin", ""),
		COMMAND_NOARGS(admin_delete_user, "Delete user", "Admin", "Delete a user from cred file and DB folder"),
		COMMAND_NOARGS(view_complaints, "View complaints", "Admin", "View number of complaints from patients, and view them"),
		
		COMMAND(make_user, "Create account", "Director", makeuser_args, "Create accounts for patients, doctors, etc\n\n First arg is username,\n second arg is usertype (cAsE sEnSiTiVe)"),
		COMMAND_NOARGS(director_view_cfg, "View profiles", "Director", "View profiles of users"),
		COMMAND_NOARGS(admin_delete_user, "Delete user", "Director", "Delete a user from cred file and DB folder"),
		COMMAND_NOARGS(calc_sales_t_range, "Sales report", "Director", "Calculate total sales between time rang"),
		COMMAND_NOARGS(view_payment_history_director, "View payment history", "Director", ""),
		
		COMMAND_NOARGS(nurse_view_booking, "View doctor's appointments", "Nurse", ""),
		COMMAND_NOARGS(inventory_edit, "View and edit inventory", "Nurse", "View and edit fields for medical supplies and inventories"),
		COMMAND_NOARGS(inventory_edit_txteditor, "Edit inventory with text editor", "Nurse", ""),
		COMMAND_NOARGS(create_note, "Write a note", "Nurse", "Create a note for a patient"),
		COMMAND_NOARGS(view_notes, "View notes", "Nurse", "Choose a patient and view notes other nurses left"),
	};

	int cmd_list_n = sizeof(cmd_list)/sizeof(command);
```


\section cmd_dispatch_full The full picture
//flowchart of system here 

The pattern of casting a function pointer to a type erased pointer, labeling it in a struct, and calling the function with the pointer later is called "manual vtable dispatch".

Places where you'll see this are CPython, the Linux kernel, and the C stdlib (For example qsort, which takes `int (*compar)(const void *, const void *)` as a param).
