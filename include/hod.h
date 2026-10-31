#pragma once
#include "user.h"
#include "moderationservice.h"
#include "auditservice.h"

class HoD : public User {
private:
    ModerationService* moderationService;
    AuditService* auditService;

    void viewAuditLog();
public:
    HoD(UserRecord record, ModerationService* moderation, AuditService* audit);
    void reviewMarks();     
    void adjustMark();      
    void releaseMarks();    
    virtual void menu();
};