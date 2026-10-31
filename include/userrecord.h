#pragma once
#include <string>
using namespace std;

class UserRecord {
public:
    int userId;
    string username;
    string passwordHash;
    string role;
    int departmentId;
    UserRecord();
};