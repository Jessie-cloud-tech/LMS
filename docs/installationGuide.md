## Installation process
-Step 1: Prepare the computer
1. Log in using an account with installation permissions.
2. Ensure that the required C++17 compiler is installed.
3. Install CMake.
4. Ensure that SQLite is available to the application.

-Step 2: Obtain the LMS
1. Obtain the approved LMS project from the school's designated software repository or installation package.
2. Extract the project into a suitable directory, for example: "C:\School\LMS"
3. Confirm that the project contains the source code, header files, CMake configuration and required resources.
--If the build completes successfully, the executable should be available in the generated build directory.

-Step 4: Configure the database
1. Confirm that the SQLite database is available or that the application can create it.
2. Ensure that the database contains the approved LMS schema.
3. Confirm that the application uses the correct database path.
4. Ensure that the application has permission to read and write to the database directory.

-Step 5: Launch the LMS

Run the generated executable from its build location.

The technician should confirm that:
- The application launches successfully.
- The login screen or login menu appears.
- Users can authenticate.
- Role-specific menus are displayed.
- Database records can be retrieved and saved.

-Step 6: Create initial accounts
The system administrator should create or verify the initial administrative account and configure the required departments, courses, lecturers and student accounts.

Default passwords, if used during setup, should be changed before the system is made available to users.

-Step 7: Back up the database
The technician should establish a regular backup procedure for the SQLite database.

Backups should be stored separately from the working database and periodically checked to confirm that they can be restored.

## Installation verification checklist
|Test	                |Expected result
|-----------------------|---------------------
|Launch application	    |LMS opens successfully
|Admin login	        |Admin menu appears
|Lecturer login  	    |Lecturer menu appears
|HoD login	HoD         |menu appears
|Student login    	    |Student menu appears
|Database connection    |Records can be retrieved
|Create a record	    |Record is saved
|Restart application    |Previously saved records remain available