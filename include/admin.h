#pragma once
#include "user.h"
#include "userRepo.h"
#include "courseRepo.h"
#include "authservice.h"
#include "auditservice.h"

class Admin : public User {
private:
    UserRepository* userRepo;
    CourseRepository* courseRepo;
    AuthService* authService;
    AuditService* auditService;

    void showDepartments();
    void createDepartment();
    void createCourse();
    void enrolStudent();
    void listUsers();
    void viewAuditLog();
public:
    Admin(UserRecord record, UserRepository* users, CourseRepository* courses,
          AuthService* auth, AuditService* audit);
    void createUser();     
    void editUser();       
    void delUser();
    void assignRole();     
    virtual void menu();
};