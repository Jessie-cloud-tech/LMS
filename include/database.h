#pragma once
#include <string>
#include <vector>
using namespace std;

struct sqlite3;

class Database {
private:
    sqlite3* db;
public:
    Database(string fileName);
    ~Database();
    void execute(string sql, vector<string> params = vector<string>());                     // INSERT, UPDATE, DELETE
    vector<vector<string> > query(string sql, vector<string> params = vector<string>());    // SELECT
    int lastInsertId();
    void begin();       // start a group of changes
    void commit();      // keep all the changes
    void rollback();    // undo all the changes
    void createTables();
};