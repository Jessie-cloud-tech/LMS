#include "admin.h"
#include "input.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Admin::Admin(UserRecord record, UserRepository* users, CourseRepository* courses,
             AuthService* auth, AuditService* audit)
    : User(record) {
    userRepo = users;
    courseRepo = courses;
    authService = auth;
    auditService = audit;
}

static string chooseRole() {
    int choice = readInt("Role: 1=Admin 2=Lecturer 3=HoD 4=Student");
    if (choice == 1) return "Admin";
    if (choice == 2) return "Lecturer";
    if (choice == 3) return "HoD";
    if (choice == 4) return "Student";
    throw runtime_error("Please choose a role from 1 to 4.");
}

void Admin::showDepartments() {
    vector<Department> list = courseRepo->listDepartments();
    cout << "Departments:\n";
    if (list.empty()) cout << "  (none yet)\n";
    for (Department d : list) {
        cout << "  [" << d.departmentId << "] " << d.departmentName << "\n";
    }
}

void Admin::createUser() {
    string name = readLine("New username");
    string pwd = readLine("Password (at least 8 characters)");
    string newRole = chooseRole();
    int dept = 0;
    if (newRole != "Admin") {
        showDepartments();
        dept = readInt("Department id");
    }
    int id = authService->registerUser(name, pwd, newRole, dept);
    cout << "User registered with id " << id << ". They can log in now.\n";
}

void Admin::listUsers() {
    vector<UserRecord> users = userRepo->listAll();
    for (UserRecord u : users) {
        cout << "  [" << u.userId << "] " << u.username << " (" << u.role
             << ") department " << u.departmentId << "\n";
    }
}

void Admin::editUser() {
    int id = readInt("User id");
    string pwd = readLine("New password (at least 8 characters)");
    authService->changePassword(id, pwd);
    cout << "Password changed.\n";
}

void Admin::delUser() {
    int id = readInt("User id to delete");
    if (id == userId) throw runtime_error("You cannot delete your own account.");
    userRepo->remove(id);
    cout << "Done.\n";
}

void Admin::assignRole() {
    int id = readInt("User id");
    if (id == userId) throw runtime_error("You cannot change your own role.");
    UserRecord user = userRepo->findById(id);
    if (user.userId == 0) throw runtime_error("User not found.");
    string newRole = chooseRole();
    int dept = 0;
    if (newRole != "Admin") dept = readInt("Department id");
    userRepo->changeRole(id, newRole, dept);
    cout << "Role changed.\n";
}

void Admin::createDepartment() {
    Department d;
    d.departmentName = readLine("Department name");
    if (d.departmentName == "") throw runtime_error("The name cannot be empty.");
    courseRepo->saveDepartment(d);
    cout << "Department created with id " << d.departmentId << ".\n";
}

void Admin::createCourse() {
    showDepartments();
    Course c;
    c.courseCode = readLine("Course code (example CS4433)");
    c.courseName = readLine("Course name");
    c.departmentId = readInt("Department id");
    if (c.courseCode == "" || c.courseName == "") throw runtime_error("Code and name cannot be empty.");
    courseRepo->save(c);
    cout << "Course created with id " << c.courseId << ".\n";
}

void Admin::enrolStudent() {
    Enrollment e;
    e.studentId = readInt("Student id");
    e.courseId = readInt("Course id");
    UserRecord user = userRepo->findById(e.studentId);
    if (user.userId == 0 || user.role != "Student") throw runtime_error("That user is not a student.");
    courseRepo->enroll(e);
    cout << "Student enrolled.\n";
}

void Admin::viewAuditLog() {
    int id = readInt("Submission id");
    vector<AuditLog> logs = auditService->history(id);
    if (logs.empty()) cout << "  (no entries)\n";
    for (AuditLog log : logs) cout << log.summary() << "\n";
}

void Admin::menu() {
    while (true) {
        cout << "\n--- Admin: " << username << " ---\n";
        cout << "1) Register new user\n";
        cout << "2) List users\n";
        cout << "3) Reset a user's password\n";
        cout << "4) Delete a user\n";
        cout << "5) Change a user's role\n";
        cout << "6) Create department\n";
        cout << "7) Create course\n";
        cout << "8) Enrol a student in a course\n";
        cout << "9) View audit log of a submission\n";
        cout << "0) Logout\n";
        int choice = readInt("Choose");
        if (choice == 0) return;
        try {
            if (choice == 1) createUser();
            else if (choice == 2) listUsers();
            else if (choice == 3) editUser();
            else if (choice == 4) delUser();
            else if (choice == 5) assignRole();
            else if (choice == 6) createDepartment();
            else if (choice == 7) createCourse();
            else if (choice == 8) enrolStudent();
            else if (choice == 9) viewAuditLog();
            else cout << "Invalid choice.\n";
        } catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}