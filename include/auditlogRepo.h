#pragma once
#include <vector>
#include "irepository.h"
#include "database.h"
#include "auditlog.h"

class AuditLogRepository : public IRepository<AuditLog> {
private:
    Database* db;
public:
    AuditLogRepository(Database* database);
    virtual AuditLog findById(int id);       
    virtual bool save(AuditLog& log);        
    virtual bool remove(int id);             
    vector<AuditLog> listBySubmission(int submissionId);
};