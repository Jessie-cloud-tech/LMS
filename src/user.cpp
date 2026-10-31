#include "user.h"
using namespace std;

User::User(UserRecord record) {
    userId = record.userId;
    username = record.username;
    passwordHash = record.passwordHash;
    role = record.role;
    departmentId = record.departmentId;
}

User::~User() {}

string User::getRole() { return role; }
int User::getUserId() { return userId; }
string User::getUsername() { return username; }
int User::getDepartmentId() { return departmentId; }