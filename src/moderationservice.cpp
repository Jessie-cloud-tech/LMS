#include "moderationservice.h"
#include "input.h"
#include <stdexcept>
using namespace std;

ModerationService::ModerationService(SubmissionRepo* s, AssignRepository* a, AuditService* au, Database* database) {
    subRepo = s;
    assignRepo = a;
    auditService = au;
    db = database;
}

vector<Submission> ModerationService::getPendingMarks(int deptId) {
    return subRepo->listPendingByDepartment(deptId);
}

void ModerationService::adjustMark(int hodId, int subId, double mark, string reason) {
    Submission s = subRepo->findById(subId);
    if (s.submissionId == 0) throw runtime_error("Submission not found.");
    if (s.status != MARKED && s.status != MODERATED) throw runtime_error("Only marks waiting for release can be adjusted.");
    Assignment a = assignRepo->findById(s.assignmentId);
    if (mark < 0 || mark > a.maxMark) throw runtime_error("The mark must be between 0 and " + to_string((int)a.maxMark) + ".");
    if (trim(reason) == "") throw runtime_error("Written reasoning is required.");

    Submission before = s;
    s.finalMark = mark;
    s.status = MODERATED;

    db->begin();
    try {
        subRepo->save(s);
        auditService->logAction(hodId, subId, before.describe(), s.describe(), reason);
        db->commit();
    } catch (...) {
        db->rollback();
        throw;
    }
}

void ModerationService::approveAndRelease(int hodId, int subId) {
    Submission s = subRepo->findById(subId);
    if (s.submissionId == 0) throw runtime_error("Submission not found.");
    if (s.status != MARKED && s.status != MODERATED) throw runtime_error("Submission " + to_string(subId) + " is not waiting for release.");

    Submission before = s;
    s.status = RELEASED;          // from now on the student can see the mark

    db->begin();
    try {
        subRepo->save(s);
        auditService->logAction(hodId, subId, before.describe(), s.describe(), "Approved for release by the Head of Department");
        db->commit();
    } catch (...) {
        db->rollback();
        throw;
    }
}