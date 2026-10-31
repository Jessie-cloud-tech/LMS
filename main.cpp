#include <iostream>
#include <stdexcept>
#include "database.h"
#include "userRepo.h"
#include "courseRepo.h"
#include "assignRepo.h"
#include "submissionRepo.h"
#include "auditlogRepo.h"
#include "standardLinearPenaltyStrategy.h"
#include "auditservice.h"
#include "authservice.h"
#include "submissionservice.h"
#include "moderationservice.h"
#include "admin.h"
#include "lecturer.h"
#include "hod.h"
#include "student.h"
#include "input.h"
using namespace std;

int main() {
    try {
        Database db("lms.db");
        db.createTables();

        // data layer
        UserRepository userRepo(&db);
        CourseRepository courseRepo(&db);
        AssignRepository assignRepo(&db);
        SubmissionRepo subRepo(&db);
        AuditLogRepository auditRepo(&db);

        // business layer
        StandardLinearPenaltyStrategy penalty;
        AuditService auditService(&auditRepo);
        AuthService authService(&userRepo);
        SubmissionService submissionService(&subRepo, &assignRepo, &courseRepo, &penalty, &auditService, &db);
        ModerationService moderationService(&subRepo, &assignRepo, &auditService, &db);

        // First run: nobody exists yet, so we create the first administrator.
        if (userRepo.countUsers() == 0) {
            cout << "First run: create the administrator account.\n";
            bool created = false;
            while (!created) {
                try {
                    string name = readLine("Admin username");
                    string pwd = readLine("Admin password (at least 8 characters)");
                    authService.registerUser(name, pwd, "Admin", 0);
                    cout << "Administrator created.\n";
                    created = true;
                } catch (exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
            }
        }

        // Only users registered by the admin can log in.
        while (true) {
            cout << "\n=== Moderated Learning Management System ===\n";
            cout << "1) Login\n";
            cout << "0) Exit\n";
            int choice = readInt("Choose");
            if (choice == 0) break;
            if (choice != 1) continue;
            try {
                string name = readLine("Username");
                string pwd = readLine("Password");
                UserRecord record = authService.login(name, pwd);   // checks the password first

                // Now that the login is valid, build the right role and give it exactly what it needs.
                User* user = 0;
                if (record.role == "Admin") {
                    user = new Admin(record, &userRepo, &courseRepo, &authService, &auditService);
                } else if (record.role == "Lecturer") {
                    user = new Lecturer(record, &courseRepo, &assignRepo, &subRepo, &submissionService);
                } else if (record.role == "HoD") {
                    user = new HoD(record, &moderationService, &auditService);
                } else if (record.role == "Student") {
                    user = new Student(record, &courseRepo, &assignRepo, &subRepo, &submissionService);
                } else {
                    throw runtime_error("Unknown role: " + record.role);
                }

                cout << "Welcome, " << user->getUsername() << " (" << user->getRole() << ")\n";
                user->menu();        // runs the Admin, Lecturer, HoD or Student menu
                delete user;
            } catch (exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }
    } catch (exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}