#pragma once
#include <vector>
#include "irepository.h"
#include "database.h"
#include "userrecord.h"

class UserRepository : public IRepository<UserRecord> {
private:
    Database* db;
public:
    UserRepository(Database* database);
    virtual UserRecord findById(int id);          
    virtual bool save(UserRecord& user);
    virtual bool remove(int id);
    UserRecord findByUsername(string username);   
    int countUsers();
    vector<UserRecord> listAll();
    bool changeRole(int id, string role, int departmentId);
};