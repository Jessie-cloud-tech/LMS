#include "auditlog.h"

AuditLog::AuditLog() {
    logId = 0;
    submissionId = 0;
    actorId = 0;
    timestamp = "";
    oldValue = "";
    newValue = "";
    reason = "";
}

string AuditLog::summary() {
    return "  #" + to_string(logId) + " " + timestamp + " | user " + to_string(actorId) +
           " | " + oldValue + "  ->  " + newValue + " | reason: " + reason;
}