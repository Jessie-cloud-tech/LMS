#pragma once
#include <string>
using namespace std;

class AuditLog {
public:
    int logId;
    int submissionId;
    int actorId;           
    string timestamp;      
    string oldValue;
    string newValue;
    string reason;
    AuditLog();
    string summary();
};