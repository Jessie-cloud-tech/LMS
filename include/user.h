#pragma once
#include <string>
#include "userrecord.h"
using namespace std;

class User {
protected:
    int userId;
    string username;
    string passwordHash;
    string role;
    int departmentId;
public:
    User(UserRecord record);
    virtual ~User();

    string getRole();
    int getUserId();
    string getUsername();
    int getDepartmentId();

    virtual void menu() = 0;
};