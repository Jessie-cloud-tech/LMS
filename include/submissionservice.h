#pragma once
#include <string>
#include "database.h"
#include "submissionRepo.h"
#include "assignRepo.h"
#include "courseRepo.h"
#include "ipenaltyStrategy.h"
#include "auditservice.h"
using namespace std;

class SubmissionService {
private:
    SubmissionRepo* subRepo;
    AssignRepository* assignRepo;
    CourseRepository* courseRepo;
    IPenaltyStrategy* penaltyStrategy;
    AuditService* auditService;
    Database* db;
public:
    SubmissionService(SubmissionRepo* subRepo, AssignRepository* assignRepo, CourseRepository* courseRepo,
                      IPenaltyStrategy* penaltyStrategy, AuditService* auditService, Database* db);
    // How many days late would this student be right now? (0 = on time)
    // A personal extension replaces the normal deadline.
    int validateDeadline(int assignmentId, int studentId);
    // Saves a new submission, or replaces the old one. Returns a message for the student.
    string uploadSubmission(Submission& sub);
    // A lecturer marks the work. The late penalty is applied automatically.
    void gradeSubmission(int lecturerId, int submissionId, double mark, string feedback, string reason);
};