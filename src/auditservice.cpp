#include "auditservice.h"
#include "datetime.h"
#include "input.h"
#include <stdexcept>
using namespace std;

AuditService::AuditService(AuditLogRepository* repo) { auditRepo = repo; }

void AuditService::logAction(int actorId, int subId, string oldVal, string newVal, string reason) {
    if (trim(reason) == "") throw runtime_error("A reason is required for every change.");
    AuditLog log;
    log.submissionId = subId;
    log.actorId = actorId;
    log.timestamp = nowText();
    log.oldValue = oldVal;
    log.newValue = newVal;
    log.reason = trim(reason);
    auditRepo->save(log);
}

vector<AuditLog> AuditService::history(int subId) {
    return auditRepo->listBySubmission(subId);
}