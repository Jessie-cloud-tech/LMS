#include "userRepo.h"
using namespace std;

static string COLUMNS = "user_id, username, password_hash, role, department_id";

static UserRecord rowToUser(vector<string> row) {
    UserRecord u;
    u.userId = stoi(row[0]);
    u.username = row[1];
    u.passwordHash = row[2];
    u.role = row[3];
    u.departmentId = stoi(row[4]);
    return u;
}

UserRepository::UserRepository(Database* database) { db = database; }

UserRecord UserRepository::findById(int id) {
    vector<vector<string> > rows = db->query("SELECT " + COLUMNS + " FROM users WHERE user_id = ?", {to_string(id)});
    if (rows.empty()) return UserRecord();
    return rowToUser(rows[0]);
}

UserRecord UserRepository::findByUsername(string username) {
    vector<vector<string> > rows = db->query("SELECT " + COLUMNS + " FROM users WHERE username = ?", {username});
    if (rows.empty()) return UserRecord();
    return rowToUser(rows[0]);
}

bool UserRepository::save(UserRecord& user) {
    if (user.userId == 0) {
        db->execute("INSERT INTO users (username, password_hash, role, department_id) VALUES (?, ?, ?, ?)",
                    {user.username, user.passwordHash, user.role, to_string(user.departmentId)});
        user.userId = db->lastInsertId();
    } else {
        db->execute("UPDATE users SET username = ?, password_hash = ?, role = ?, department_id = ? WHERE user_id = ?",
                    {user.username, user.passwordHash, user.role, to_string(user.departmentId), to_string(user.userId)});
    }
    return true;
}

bool UserRepository::remove(int id) {
    db->execute("DELETE FROM users WHERE user_id = ?", {to_string(id)});
    return true;
}

int UserRepository::countUsers() {
    vector<vector<string> > rows = db->query("SELECT COUNT(*) FROM users");
    return stoi(rows[0][0]);
}

vector<UserRecord> UserRepository::listAll() {
    vector<vector<string> > rows = db->query("SELECT " + COLUMNS + " FROM users ORDER BY user_id");
    vector<UserRecord> list;
    for (vector<string> row : rows) list.push_back(rowToUser(row));
    return list;
}

bool UserRepository::changeRole(int id, string role, int departmentId) {
    db->execute("UPDATE users SET role = ?, department_id = ? WHERE user_id = ?",
                {role, to_string(departmentId), to_string(id)});
    return true;
}