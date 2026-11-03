UML diagram

+---------------------------------------------------------------+
|                    School Workstation / PC                    |
|                                                               |
|  +---------------------------------------------------------+  |
|  |               C++17 LMS Application /OS                 |  |
|  |  +-------------------+  +----------------------------+  |  |
|  |  | Presentation Layer|  |  Business Logic & Services |  |  |
|  |  | (Console / GUI)   |  |  (Penalty, Auth, Audit)    |  |  |
|  |  +-------------------+  +----------------------------+  |  |
|  |                         +----------------------------+  |  |
|  |                         | Data Access Layer (DAO)    |  |  |
|  |                         +----------------------------+  |  |
|  +---------------------------------------------------------+  |
|                               | (Local File I/O)              |
|                               v                               |
|  +---------------------------------------------------------+  |
|  |               Embedded SQLite Database                  |  |
|  |               (`lms_school_database.db`)                |  |
|  +---------------------------------------------------------+  |
+---------------------------------------------------------------+

## Diagram explanation
-School Computer: The physical computer on which the LMS is installed.

-LMS Application: The compiled C++17 executable.

-Role-Based User Interface: Displays the appropriate menus and functions according to the logged-in user's role.

-Authentication Service: Verifies user credentials and controls access.

-Business Logic Layer: Handles assignment creation, submissions, extensions, marking, penalties and moderation.

-Data Access Layer: Communicates with the database using SQL queries.

-SQLite Database: Stores users, departments, courses, assignments, submissions, extensions, marks and audit records.