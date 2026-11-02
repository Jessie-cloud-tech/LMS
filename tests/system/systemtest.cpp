#include "lms.h"

// SYSTEM TEST 01
// Complete workflow: submission, marking, moderation and release.
TEST_F(LmsH, System_CompleteSubmissionToReleaseWorkflow) {
    // Student submits assignment.
    Submission s = submit(studentId);

    ASSERT_GT(s.submissionId, 0);
    EXPECT_EQ(subRepo.findById(s.submissionId).status, SUBMITTED);

    // Lecturer marks the submission.
    submissionService.gradeSubmission(
        lecturerId, s.submissionId, 80, "Good work", ""
    );

    Submission marked = subRepo.findById(s.submissionId);

    EXPECT_EQ(marked.status, MARKED);
    EXPECT_DOUBLE_EQ(marked.rawMark, 80);
    EXPECT_DOUBLE_EQ(marked.finalMark, 80);

    // Mark must not be released yet.
    EXPECT_EQ(marked.status, MARKED);

    // HoD approves and releases the mark.
    moderationService.approveAndRelease(hodId, s.submissionId);

    Submission released = subRepo.findById(s.submissionId);

    EXPECT_EQ(released.status, RELEASED);
    EXPECT_DOUBLE_EQ(released.finalMark, 80);
    EXPECT_EQ(released.feedback, "Good work");

    // Verify that the workflow was audited.
    vector<AuditLog> history = auditService.history(s.submissionId);

    ASSERT_EQ(history.size(), 2u);
    EXPECT_EQ(history[0].actorId, lecturerId);
    EXPECT_EQ(history[1].actorId, hodId);
}


// SYSTEM TEST 02
// Late submission, penalty calculation and final mark.
TEST_F(LmsH, System_LateSubmissionPenaltySurvivesMarkingAndRelease) {
    // Submit one second after the deadline.
    setNow(1);

    Submission s = submit(studentId);

    ASSERT_GT(s.submissionId, 0);
    EXPECT_EQ(s.status, LATE);
    EXPECT_EQ(s.daysLate, 1);

    // Lecturer awards 80 before the penalty.
    submissionService.gradeSubmission(
        lecturerId, s.submissionId, 80, "Good work", ""
    );

    Submission marked = subRepo.findById(s.submissionId);

    EXPECT_DOUBLE_EQ(marked.rawMark, 80);
    EXPECT_DOUBLE_EQ(marked.finalMark, 72);

    // HoD releases the result.
    moderationService.approveAndRelease(hodId, s.submissionId);

    Submission released = subRepo.findById(s.submissionId);

    EXPECT_EQ(released.status, RELEASED);
    EXPECT_DOUBLE_EQ(released.rawMark, 80);
    EXPECT_DOUBLE_EQ(released.finalMark, 72);
}


// SYSTEM TEST 03
// HoD moderates a lecturer's mark and the change is audited.
TEST_F(LmsH, System_ModerationChangesOfficialMark) {
    Submission s = submit(studentId);

    // Lecturer initially awards 80.
    submissionService.gradeSubmission(
        lecturerId, s.submissionId, 80, "Good work", ""
    );

    // HoD adjusts the mark to 75.
    moderationService.adjustMark(
        hodId, s.submissionId, 75, "Moderation adjustment"
    );

    Submission moderated = subRepo.findById(s.submissionId);

    EXPECT_EQ(moderated.status, MODERATED);
    EXPECT_DOUBLE_EQ(moderated.rawMark, 80);
    EXPECT_DOUBLE_EQ(moderated.finalMark, 75);

    // Release the moderated result.
    moderationService.approveAndRelease(hodId, s.submissionId);

    Submission released = subRepo.findById(s.submissionId);

    EXPECT_EQ(released.status, RELEASED);
    EXPECT_DOUBLE_EQ(released.finalMark, 75);

    // Confirm both changes were recorded.
    vector<AuditLog> history = auditService.history(s.submissionId);

    ASSERT_EQ(history.size(), 3u);

    EXPECT_EQ(history[0].actorId, lecturerId);
    EXPECT_EQ(history[1].actorId, hodId);
    EXPECT_EQ(history[1].reason, "Moderation adjustment");
    EXPECT_EQ(history[2].actorId, hodId);
}


// SYSTEM TEST 04
// A student's extension applies only to that student.
TEST_F(LmsH, System_ExtensionAllowsOneStudentToSubmitAfterNormalDeadline) {
    // Move time five days past the original deadline.
    setNow(5 * 86400);

    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = studentId;
    e.extDeadline = secondsToText(due + 7 * 86400);
    e.reason = "Approved extension";

    assignRepo.saveExtension(e);

    // The student with the extension can submit.
    Submission s = submit(studentId);

    EXPECT_EQ(s.status, SUBMITTED);
    EXPECT_EQ(s.daysLate, 0);

    // The second student has no extension and cannot submit.
    Submission s2;
    s2.assignmentId = assignmentId;
    s2.studentId = student2Id;
    s2.filePath = uploadFile;

    EXPECT_THROW(
        submissionService.uploadSubmission(s2),
        runtime_error
    );
}
