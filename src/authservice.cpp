#include "authservice.h"
#include "password.h"
#include <cctype>
#include <stdexcept>
using namespace std;

string makeHash(string pwd);

static void checkUsername(string name) {
    if (name.size() < 3 || name.size() > 32) throw runtime_error("The username must be 3 to 32 characters long.");
    for (char c : name) {
        if (!isalnum((unsigned char)c) && c != '_' && c != '.') {
            throw runtime_error("The username may only contain letters, digits, _ and .");
        }
    }
}

static void checkPassword(string pwd) {
    if (pwd.size() < 8 || pwd.size() > 128) throw runtime_error("The password must be 8 to 128 characters long.");
}

AuthService::AuthService(UserRepository* repo) { userRepo = repo; }

UserRecord AuthService::login(string username, string pwd) {
    UserRecord record = userRepo->findByUsername(username);
    if (record.userId == 0 || !passwordMatches(record.passwordHash, pwd)) {
        throw runtime_error("Invalid username or password.");    // same message for both cases
    }
    return record;
}

string AuthService::hashPassword(string pwd) { return makeHash(pwd); }

int AuthService::registerUser(string username, string pwd, string role, int departmentId) {
    checkUsername(username);
    checkPassword(pwd);
    if (role != "Admin" && departmentId <= 0) throw runtime_error("Please choose a valid department.");

    UserRecord existing = userRepo->findByUsername(username);
    if (existing.userId != 0) throw runtime_error("That username is already taken.");

    UserRecord record;
    record.username = username;
    record.passwordHash = hashPassword(pwd);
    record.role = role;
    record.departmentId = departmentId;
    userRepo->save(record);
    return record.userId;
}

void AuthService::changePassword(int userId, string newPwd) {
    checkPassword(newPwd);
    UserRecord record = userRepo->findById(userId);
    if (record.userId == 0) throw runtime_error("User not found.");
    record.passwordHash = hashPassword(newPwd);
    userRepo->save(record);
}