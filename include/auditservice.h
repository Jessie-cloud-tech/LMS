#pragma once
#include <string>
#include <vector>
#include "auditlogRepo.h"
using namespace std;

class AuditService {
private:
    AuditLogRepository* auditRepo;
public:
    AuditService(AuditLogRepository* repo);
    void logAction(int actorId, int subId, string oldVal, string newVal, string reason);
    vector<AuditLog> history(int subId);
};