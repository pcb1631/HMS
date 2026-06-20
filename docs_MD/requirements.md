\page requirements Project requirements

Fuzzy search with levenshtein distance is built into \ref simple_select()
\n and \ref user_select_menu() calls \ref simple_select()

Director
- Manage admin (add, search, view)
- Staff report
	- **This is implemented as profile and description**
- Sales report
	- **The director can get a report between a time range, not just the past year**

Admin
- Add patients, doctors, nurse
- View Doctor and Nurse report
	- **This is implemented as profile and description**
- View patient complaints

Patient
- View, book, cancel appointments
	- **It is decided that the time of appointment slots is strictly controlled by the doctor, otherwise the patient can communicate with the doctor externally**
-	Pay, view and search billing
- Complaints

Doctor
- Doctor Scheduler
	- add, view, edit
	- **This is implemented as booking, booking options, or appointment slots**
- Diagnostic reports

Nurse
- View appointments only
- Meds and Supply inventory stock view
	- **One config file for stock in DB root, edited with text_editor() or config_editor()**
- Notes
