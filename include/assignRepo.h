#pragma once
#include <vector>
#include "irepository.h"
#include "database.h"
#include "assignment.h"
#include "extension.h"

class AssignRepository : public IRepository<Assignment> {
private:
    Database* db;
public:
    AssignRepository(Database* database);
    virtual Assignment findById(int id);       // assignmentId is 0 when nothing is found
    virtual bool save(Assignment& assignment);
    virtual bool remove(int id);
    vector<Assignment> listByCourse(int courseId);
    bool saveExtension(Extension& extension);
    Extension findExtension(int assignmentId, int studentId);
};