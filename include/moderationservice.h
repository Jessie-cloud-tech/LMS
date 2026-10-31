#pragma once
#include <string>
#include <vector>
#include "database.h"
#include "submissionRepo.h"
#include "assignRepo.h"
#include "auditservice.h"
using namespace std;

class ModerationService {
private:
    SubmissionRepo* subRepo;
    AssignRepository* assignRepo;
    AuditService* auditService;
    Database* db;
public:
    ModerationService(SubmissionRepo* subRepo, AssignRepository* assignRepo, AuditService* auditService, Database* db);
    vector<Submission> getPendingMarks(int deptId);                      // marks waiting for approval
    void adjustMark(int hodId, int subId, double mark, string reason);
    void approveAndRelease(int hodId, int subId);
};