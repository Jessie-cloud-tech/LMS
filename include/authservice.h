#pragma once
#include <string>
#include "userRepo.h"
using namespace std;

class AuthService {
private:
    UserRepository* userRepo;
public:
    AuthService(UserRepository* repo);
    UserRecord login(string username, string pwd);    
    string hashPassword(string pwd);
    int registerUser(string username, string pwd, string role, int departmentId);
    void changePassword(int userId, string newPwd);
};