#include "database.h"
#include <sqlite3.h>
#include <stdexcept>
using namespace std;

// Prepares the SQL and fills in the ? marks.
static sqlite3_stmt* prepare(sqlite3* db, string sql, vector<string> params) {
    sqlite3_stmt* st = 0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &st, 0) != SQLITE_OK) {
        throw runtime_error(string("SQL error: ") + sqlite3_errmsg(db));
    }
    for (int i = 0; i < (int)params.size(); i++) {
        sqlite3_bind_text(st, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    return st;
}

Database::Database(string fileName) {
    db = 0;
    if (sqlite3_open(fileName.c_str(), &db) != SQLITE_OK) {
        sqlite3_close(db);
        throw runtime_error("Cannot open the database file.");
    }
    execute("PRAGMA foreign_keys = ON");
}

Database::~Database() { sqlite3_close(db); }

void Database::execute(string sql, vector<string> params) {
    sqlite3_stmt* st = prepare(db, sql, params);
    int result = sqlite3_step(st);
    string message = sqlite3_errmsg(db);
    sqlite3_finalize(st);
    if (result != SQLITE_DONE) throw runtime_error("Database error: " + message);
}

vector<vector<string> > Database::query(string sql, vector<string> params) {
    sqlite3_stmt* st = prepare(db, sql, params);
    vector<vector<string> > rows;
    while (sqlite3_step(st) == SQLITE_ROW) {
        vector<string> row;
        for (int i = 0; i < sqlite3_column_count(st); i++) {
            const unsigned char* text = sqlite3_column_text(st, i);
            if (text == 0) row.push_back("");
            else row.push_back((const char*)text);
        }
        rows.push_back(row);
    }
    sqlite3_finalize(st);
    return rows;
}

int Database::lastInsertId() { return (int)sqlite3_last_insert_rowid(db); }
void Database::begin() { execute("BEGIN"); }
void Database::commit() { execute("COMMIT"); }
void Database::rollback() { execute("ROLLBACK"); }

void Database::createTables() {
    execute("CREATE TABLE IF NOT EXISTS departments ("
            "department_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "department_name TEXT NOT NULL UNIQUE)");

    execute("CREATE TABLE IF NOT EXISTS users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT NOT NULL UNIQUE, "
            "password_hash TEXT NOT NULL, "
            "role TEXT NOT NULL CHECK (role IN ('Admin','Lecturer','HoD','Student')), "
            "department_id INTEGER NOT NULL DEFAULT 0)");

    execute("CREATE TABLE IF NOT EXISTS courses ("
            "course_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "course_code TEXT NOT NULL UNIQUE, "
            "course_name TEXT NOT NULL, "
            "department_id INTEGER NOT NULL REFERENCES departments(department_id))");

    execute("CREATE TABLE IF NOT EXISTS enrollments ("
            "enrollment_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "student_id INTEGER NOT NULL REFERENCES users(user_id), "
            "course_id INTEGER NOT NULL REFERENCES courses(course_id), "
            "UNIQUE (student_id, course_id))");

    execute("CREATE TABLE IF NOT EXISTS assignments ("
            "assignment_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "course_id INTEGER NOT NULL REFERENCES courses(course_id), "
            "title TEXT NOT NULL, "
            "description TEXT NOT NULL, "
            "deadline TEXT NOT NULL, "
            "max_mark REAL NOT NULL CHECK (max_mark > 0))");

    execute("CREATE TABLE IF NOT EXISTS extensions ("
            "extension_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "student_id INTEGER NOT NULL REFERENCES users(user_id), "
            "assignment_id INTEGER NOT NULL REFERENCES assignments(assignment_id), "
            "ext_deadline TEXT NOT NULL, "
            "reason TEXT NOT NULL)");

    // raw_mark and final_mark are -1 until the lecturer marks the work
    execute("CREATE TABLE IF NOT EXISTS submissions ("
            "submission_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "assignment_id INTEGER NOT NULL REFERENCES assignments(assignment_id), "
            "student_id INTEGER NOT NULL REFERENCES users(user_id), "
            "file_path TEXT NOT NULL, "
            "sub_date TEXT NOT NULL, "
            "status TEXT NOT NULL CHECK (status IN ('Submitted','Late','Marked','Moderated','Released')), "
            "raw_mark REAL NOT NULL DEFAULT -1, "
            "final_mark REAL NOT NULL DEFAULT -1, "
            "feedback TEXT NOT NULL DEFAULT '', "
            "days_late INTEGER NOT NULL DEFAULT 0, "
            "UNIQUE (assignment_id, student_id))");

    execute("CREATE TABLE IF NOT EXISTS audit_logs ("
            "log_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "submission_id INTEGER NOT NULL REFERENCES submissions(submission_id), "
            "actor_id INTEGER NOT NULL REFERENCES users(user_id), "
            "logged_at TEXT NOT NULL, "
            "old_value TEXT NOT NULL, "
            "new_value TEXT NOT NULL, "
            "reason TEXT NOT NULL CHECK (length(trim(reason)) > 0))");
}