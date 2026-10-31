#pragma once
#include <vector>
#include "irepository.h"
#include "database.h"
#include "submission.h"

class SubmissionRepo : public IRepository<Submission> {
private:
    Database* db;
public:
    SubmissionRepo(Database* database);
    virtual Submission findById(int id);        
    virtual bool save(Submission& submission);
    virtual bool remove(int id);
    Submission find(int assignmentId, int studentId);
    vector<Submission> listByAssignment(int assignmentId);
    vector<Submission> listByStudent(int studentId);
    vector<Submission> listPendingByDepartment(int departmentId);    
};