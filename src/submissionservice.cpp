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

    if (a.assignmentId == 0) {
        throw runtime_error("Assignment not found.");
    }

    // Uses individual extension first, then class extension,
    // then the original assignment deadline.
    string deadline = getEffectiveDeadline(assignmentId, studentId);

    long long due = toSeconds(deadline);
    long long now = nowSeconds();

    if (now <= due) {
        return 0;
    }

    // A started late day counts as a full day.
    return static_cast<int>((now - due + 86399) / 86400);
}

string SubmissionService::uploadSubmission(Submission& submission) {
    Assignment a = assignRepo->findById(submission.assignmentId);
    if (a.assignmentId == 0) throw runtime_error("Assignment not found.");
    if (!courseRepo->isEnrolled(submission.studentId, a.courseId)) throw runtime_error("You are not enrolled in this course.");

    ifstream file(submission.filePath.c_str());
    if (!file) throw runtime_error("File not found: " + submission.filePath);

    int late = validateDeadline(submission.assignmentId, submission.studentId);
    double penalty = penaltyStrategy->calculatePenalty(late);
    if (penalty >= 100) throw runtime_error("The submission window is closed (more than 3 days late).");

    Submission old = subRepo->find(submission.assignmentId, submission.studentId);
    if (old.submissionId != 0) {                              // this is a resubmission
        if (late > 0) throw runtime_error("You can only resubmit before the deadline.");
        if (old.status != SUBMITTED && old.status != LATE) throw runtime_error("Your work is already marked, so it cannot be replaced.");
    }

    submission.submissionId = old.submissionId;      // 0 = new row, otherwise the old row is replaced
    submission.subDate = nowText();
    submission.daysLate = late;
    submission.status = (late > 0) ? LATE : SUBMITTED;
    submission.rawMark = -1;
    submission.finalMark = -1;
    submission.feedback = "";
    subRepo->save(submission);

    // Keep the generated database ID on the caller's object. The lookup is a
    // defensive fallback in case a repository implementation inserts the row
    // but does not propagate lastInsertId() to the supplied object.
    if (submission.submissionId == 0) {
        Submission saved = subRepo->find(submission.assignmentId, submission.studentId);
        submission.submissionId = saved.submissionId;
    }
    if (submission.submissionId == 0) {
        throw runtime_error("The submission could not be saved.");
    }

    if (late > 0) return "Submitted LATE by " + to_string(late) + " day(s). A " + to_string((int)penalty) + "% penalty will be applied.";
    if (old.submissionId != 0) return "Your previous submission was replaced.";
    return "Submission received on time.";
}
string SubmissionService::getEffectiveDeadline(int assignmentId, int studentId) {
    // Phase 8 precedence: an individual extension overrides a class extension.
    Extension individual = assignRepo->findExtension(assignmentId, studentId);
    if (individual.extensionId != 0 && !individual.extDeadline.empty()) {
        return individual.extDeadline;
    }

    // If there is no individual extension, use the class-wide deadline.
    string classDeadline;
    if (assignRepo->findClassExtension(assignmentId, classDeadline) &&
        !classDeadline.empty()) {
        return classDeadline;
    }

    // Otherwise, use the assignment's original deadline.
    Assignment assignment = assignRepo->findById(assignmentId);
    if (assignment.assignmentId == 0) {
        throw runtime_error("Assignment not found.");
    }
    return assignment.deadline;
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