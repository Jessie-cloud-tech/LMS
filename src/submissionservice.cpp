#include "submissionservice.h"
#include "datetime.h"
#include "input.h"
#include <fstream>
#include <stdexcept>
using namespace std;

SubmissionService::SubmissionService(SubmissionRepo* s, AssignRepository* a, CourseRepository* c,
                                     IPenaltyStrategy* p, AuditService* au, Database* database) {
    subRepo = s;
    assignRepo = a;
    courseRepo = c;
    penaltyStrategy = p;
    auditService = au;
    db = database;
}

int SubmissionService::validateDeadline(int assignmentId, int studentId) {
    Assignment a = assignRepo->findById(assignmentId);
    if (a.assignmentId == 0) throw runtime_error("Assignment not found.");

    string deadline = a.deadline;
    Extension ext = assignRepo->findExtension(assignmentId, studentId);
    if (ext.extensionId != 0) deadline = ext.extDeadline;     // the extension wins

    long long due = toSeconds(deadline);
    long long now = nowSeconds();
    if (now <= due) return 0;
    return (int)((now - due + 86399) / 86400);                // a started day counts as a full day
}

string SubmissionService::uploadSubmission(Submission& sub) {
    Assignment a = assignRepo->findById(sub.assignmentId);
    if (a.assignmentId == 0) throw runtime_error("Assignment not found.");
    if (!courseRepo->isEnrolled(sub.studentId, a.courseId)) throw runtime_error("You are not enrolled in this course.");

    ifstream file(sub.filePath.c_str());
    if (!file) throw runtime_error("File not found: " + sub.filePath);

    int late = validateDeadline(sub.assignmentId, sub.studentId);
    double penalty = penaltyStrategy->calculatePenalty(late);
    if (penalty >= 100) throw runtime_error("The submission window is closed (more than 3 days late).");

    Submission old = subRepo->find(sub.assignmentId, sub.studentId);
    if (old.submissionId != 0) {                              // this is a resubmission
        if (late > 0) throw runtime_error("You can only resubmit before the deadline.");
        if (old.status != SUBMITTED && old.status != LATE) throw runtime_error("Your work is already marked, so it cannot be replaced.");
    }

    sub.submissionId = old.submissionId;      // 0 = new row, otherwise the old row is replaced
    sub.subDate = nowText();
    sub.daysLate = late;
    sub.status = (late > 0) ? LATE : SUBMITTED;
    sub.rawMark = -1;
    sub.finalMark = -1;
    sub.feedback = "";
    subRepo->save(sub);

    if (late > 0) return "Submitted LATE by " + to_string(late) + " day(s). A " + to_string((int)penalty) + "% penalty will be applied.";
    if (old.submissionId != 0) return "Your previous submission was replaced.";
    return "Submission received on time.";
}

void SubmissionService::gradeSubmission(int lecturerId, int submissionId, double mark, string feedback, string reason) {
    Submission s = subRepo->findById(submissionId);
    if (s.submissionId == 0) throw runtime_error("Submission not found.");
    if (s.status != SUBMITTED && s.status != LATE && s.status != MARKED) {
        throw runtime_error("This submission is already " + statusToString(s.status) + " and cannot be marked.");
    }
    Assignment a = assignRepo->findById(s.assignmentId);
    if (mark < 0 || mark > a.maxMark) throw runtime_error("The mark must be between 0 and " + to_string((int)a.maxMark) + ".");
    if (trim(feedback) == "") throw runtime_error("Written feedback is required.");

    bool isChange = (s.status == MARKED);                     // marking again = changing a mark
    string why = trim(reason);
    if (isChange && why == "") throw runtime_error("A reason is required when you change a mark.");
    if (!isChange) why = "Initial marking";

    Submission before = s;
    double penalty = penaltyStrategy->calculatePenalty(s.daysLate);
    s.rawMark = mark;
    s.feedback = trim(feedback);
    s.finalMark = (int)(mark * (100 - penalty) + 0.5) / 100.0;   // mark after the penalty, 2 decimals
    s.status = MARKED;

    // save the mark and the audit entry together: both happen or neither happens
    db->begin();
    try {
        subRepo->save(s);
        auditService->logAction(lecturerId, s.submissionId, before.describe(), s.describe(), why);
        db->commit();
    } catch (...) {
        db->rollback();
        throw;
    }
}