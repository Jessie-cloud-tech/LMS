#include "lms.h"

//  AuthService (FR-01, NFR-06) 
TEST_F(LmsH, Auth_RegisterThenLoginReturnsTheRecord) {
    int id = authService.registerUser("newlect", "password1", "Lecturer", deptId);
    EXPECT_GT(id, 0);
    UserRecord r = authService.login("newlect", "password1");
    EXPECT_EQ(r.userId, id);
    EXPECT_EQ(r.role, "Lecturer");
    EXPECT_EQ(r.departmentId, deptId);
}

TEST_F(LmsH, Auth_PasswordsAreHashedAndSalted) {
    authService.registerUser("alice", "password1", "Student", deptId);
    authService.registerUser("bob", "password1", "Student", deptId);
    string h1 = userRepo.findByUsername("alice").passwordHash;
    string h2 = userRepo.findByUsername("bob").passwordHash;
    EXPECT_EQ(h1.rfind("$argon2", 0), 0u);
    EXPECT_NE(h1, "password1");
    EXPECT_NE(h1, h2);                     // same password, different salt
    EXPECT_NE(authService.hashPassword("password1"), authService.hashPassword("password1"));
}

TEST_F(LmsH, Auth_PasswordLengthBoundaries) {
    EXPECT_THROW(authService.registerUser("user_a", string(7, 'a'), "Student", deptId), runtime_error);
    EXPECT_NO_THROW(authService.registerUser("user_b", string(8, 'a'), "Student", deptId));
    EXPECT_NO_THROW(authService.registerUser("user_c", string(128, 'a'), "Student", deptId));
    EXPECT_THROW(authService.registerUser("user_d", string(129, 'a'), "Student", deptId), runtime_error);
}

TEST_F(LmsH, Auth_UsernameBoundariesAndCharacters) {
    EXPECT_THROW(authService.registerUser(string(2, 'a'), "password1", "Student", deptId), runtime_error);
    EXPECT_NO_THROW(authService.registerUser(string(3, 'a'), "password1", "Student", deptId));
    EXPECT_NO_THROW(authService.registerUser(string(32, 'b'), "password1", "Student", deptId));
    EXPECT_THROW(authService.registerUser(string(33, 'c'), "password1", "Student", deptId), runtime_error);
    EXPECT_THROW(authService.registerUser("bad name", "password1", "Student", deptId), runtime_error);
    EXPECT_THROW(authService.registerUser("bad@name", "password1", "Student", deptId), runtime_error);
    EXPECT_NO_THROW(authService.registerUser("ok_name.1", "password1", "Student", deptId));
}

TEST_F(LmsH, Auth_RejectsDuplicateUsername) {
    authService.registerUser("same", "password1", "Student", deptId);
    EXPECT_THROW(authService.registerUser("same", "password2", "Student", deptId), runtime_error);
}

TEST_F(LmsH, Auth_DepartmentRules) {
    EXPECT_THROW(authService.registerUser("nodept", "password1", "Student", 0), runtime_error);
    EXPECT_NO_THROW(authService.registerUser("admin2", "password1", "Admin", 0));
}

TEST_F(LmsH, Auth_WrongPasswordAndUnknownUserGiveTheSameMessage) {
    authService.registerUser("realuser", "password1", "Student", deptId);
    string m1, m2;
    try { authService.login("realuser", "wrongpass1"); } catch (runtime_error& e) { m1 = e.what(); }
    try { authService.login("ghost", "password1"); } catch (runtime_error& e) { m2 = e.what(); }
    EXPECT_FALSE(m1.empty());
    EXPECT_EQ(m1, m2);                    // does not reveal which usernames exist
}

TEST_F(LmsH, Auth_ChangePassword) {
    authService.registerUser("changer", "password1", "Student", deptId);
    int id = userRepo.findByUsername("changer").userId;
    authService.changePassword(id, "newpassword1");
    EXPECT_THROW(authService.login("changer", "password1"), runtime_error);
    EXPECT_NO_THROW(authService.login("changer", "newpassword1"));
    EXPECT_THROW(authService.changePassword(id, "short"), runtime_error);
    EXPECT_THROW(authService.changePassword(9999, "newpassword1"), runtime_error);
}

//  AuditService (FR-10) 
TEST_F(LmsH, Audit_ReasonIsMandatory) {
    Submission s = submit(studentId);
    EXPECT_THROW(auditService.logAction(lecturerId, s.submissionId, "a", "b", ""), runtime_error);
    EXPECT_THROW(auditService.logAction(lecturerId, s.submissionId, "a", "b", "   "), runtime_error);
    EXPECT_TRUE(auditService.history(s.submissionId).empty());
}

TEST_F(LmsH, Audit_RecordsWhoWhenOldNewAndReason) {
    setNow(0);
    Submission s = submit(studentId);
    auditService.logAction(lecturerId, s.submissionId, "old value", "new value", "  because  ");
    auditService.logAction(hodId, s.submissionId, "second old", "second new", "again");
    vector<AuditLog> h = auditService.history(s.submissionId);
    ASSERT_EQ(h.size(), 2u);
    EXPECT_EQ(h[0].actorId, lecturerId);
    EXPECT_EQ(h[0].oldValue, "old value");
    EXPECT_EQ(h[0].newValue, "new value");
    EXPECT_EQ(h[0].reason, "because");                 // spaces trimmed
    EXPECT_EQ(h[0].timestamp, secondsToText(due));     // the (fake) current time
    EXPECT_EQ(h[1].actorId, hodId);                    // oldest first
}

//  SubmissionService: submitting (UC-07, FR-09, FR-11) 
TEST_F(LmsH, Submit_OnTimeMessageAndStatus) {
    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = studentId;
    s.filePath = uploadFile;
    EXPECT_EQ(submissionService.uploadSubmission(s), "Submission received on time.");
    EXPECT_GT(s.submissionId, 0);
    EXPECT_EQ(s.status, SUBMITTED);
}

TEST_F(LmsH, Submit_LateMessageNamesDaysAndPenalty) {
    setNow(86400 + 1);                                  // 2 started days late
    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = studentId;
    s.filePath = uploadFile;
    EXPECT_EQ(submissionService.uploadSubmission(s),
              "Submitted LATE by 2 day(s). A 20% penalty will be applied.");
}

struct LateCase {
    long long offset;      // seconds after the deadline (negative = before)
    int days;
    double penalty;
    bool accepted;
};
class LatenessTest : public LmsH, public ::testing::WithParamInterface<LateCase> {};

TEST_P(LatenessTest, DeadlineBoundaries) {
    LateCase c = GetParam();
    setNow(c.offset);
    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = studentId;
    s.filePath = uploadFile;
    if (!c.accepted) {
        EXPECT_THROW(submissionService.uploadSubmission(s), runtime_error);
        EXPECT_EQ(subRepo.find(assignmentId, studentId).submissionId, 0);   // nothing was saved
        return;
    }
    submissionService.uploadSubmission(s);
    Submission saved = subRepo.find(assignmentId, studentId);
    EXPECT_EQ(saved.daysLate, c.days);
    EXPECT_EQ(saved.status, c.days == 0 ? SUBMITTED : LATE);
    EXPECT_EQ(submissionService.validateDeadline(assignmentId, studentId), c.days);
    EXPECT_DOUBLE_EQ(penalty.calculatePenalty(saved.daysLate), c.penalty);
}

INSTANTIATE_TEST_SUITE_P(Boundaries, LatenessTest, ::testing::Values(
    LateCase{-86400, 0, 0, true},         // a day early
    LateCase{-1, 0, 0, true},             // one second before
    LateCase{0, 0, 0, true},              // exactly at the deadline
    LateCase{1, 1, 10, true},             // one second late
    LateCase{86400, 1, 10, true},         // exactly one day late
    LateCase{86401, 2, 20, true},
    LateCase{2 * 86400, 2, 20, true},
    LateCase{2 * 86400 + 1, 3, 30, true},
    LateCase{3 * 86400, 3, 30, true},     // last accepted instant
    LateCase{3 * 86400 + 1, 4, 100, false},   // first rejected instant
    LateCase{10 * 86400, 10, 100, false}));

TEST_F(LmsH, Submit_RejectsInvalidRequests) {
    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = outsiderId;                            // not enrolled
    s.filePath = uploadFile;
    EXPECT_THROW(submissionService.uploadSubmission(s), runtime_error);

    s.studentId = studentId;
    s.filePath = "no_such_file.txt";                     // file missing
    EXPECT_THROW(submissionService.uploadSubmission(s), runtime_error);

    s.filePath = uploadFile;
    s.assignmentId = 9999;                               // unknown assignment
    EXPECT_THROW(submissionService.uploadSubmission(s), runtime_error);
    EXPECT_THROW(submissionService.validateDeadline(9999, studentId), runtime_error);
}

TEST_F(LmsH, Submit_ResubmissionBeforeTheDeadlineReplacesTheOldRow) {
    Submission first = submit(studentId);
    Submission again;
    again.assignmentId = assignmentId;
    again.studentId = studentId;
    again.filePath = uploadFile;
    EXPECT_EQ(submissionService.uploadSubmission(again), "Your previous submission was replaced.");
    EXPECT_EQ(again.submissionId, first.submissionId);               // same row: only the last counts
    EXPECT_EQ(subRepo.listByStudent(studentId).size(), 1u);
}

TEST_F(LmsH, Submit_ResubmissionAfterTheDeadlineIsBlocked) {
    submit(studentId);
    setNow(10);                                          // now just after the deadline
    Submission again;
    again.assignmentId = assignmentId;
    again.studentId = studentId;
    again.filePath = uploadFile;
    EXPECT_THROW(submissionService.uploadSubmission(again), runtime_error);
}

TEST_F(LmsH, Submit_ResubmissionAfterMarkingIsBlocked) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    Submission again;
    again.assignmentId = assignmentId;
    again.studentId = studentId;
    again.filePath = uploadFile;
    EXPECT_THROW(submissionService.uploadSubmission(again), runtime_error);
}

TEST_F(LmsH, Submit_ExtensionMovesTheDeadlineForOneStudentOnly) {
    setNow(5 * 86400);                                   // five days after the normal deadline
    Submission s;
    s.assignmentId = assignmentId;
    s.studentId = studentId;
    s.filePath = uploadFile;
    EXPECT_THROW(submissionService.uploadSubmission(s), runtime_error);   // closed without extension

    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = studentId;
    e.extDeadline = secondsToText(due + 7 * 86400);
    e.reason = "Illness";
    assignRepo.saveExtension(e);

    EXPECT_EQ(submissionService.validateDeadline(assignmentId, studentId), 0);
    EXPECT_NO_THROW(submissionService.uploadSubmission(s));
    EXPECT_EQ(subRepo.find(assignmentId, studentId).status, SUBMITTED);

    EXPECT_EQ(submissionService.validateDeadline(assignmentId, student2Id), 5);  // no extension
    Submission s2;
    s2.assignmentId = assignmentId;
    s2.studentId = student2Id;
    s2.filePath = uploadFile;
    EXPECT_THROW(submissionService.uploadSubmission(s2), runtime_error);
}

TEST_F(LmsH, Submit_LatenessIsMeasuredFromTheExtension) {
    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = studentId;
    e.extDeadline = secondsToText(due + 2 * 86400);
    e.reason = "Illness";
    assignRepo.saveExtension(e);
    setNow(2 * 86400 + 60);                              // a minute after the extended deadline
    EXPECT_EQ(submissionService.validateDeadline(assignmentId, studentId), 1);
    Submission s = submit(studentId);
    EXPECT_EQ(s.status, LATE);
}

//  SubmissionService: marking (UC-04, FR-05, FR-10) 
TEST_F(LmsH, Grade_FirstMarkOnTimeIsAuditedWithOldAndNewValues) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good work", "");
    Submission m = subRepo.findById(s.submissionId);
    EXPECT_EQ(m.status, MARKED);
    EXPECT_DOUBLE_EQ(m.rawMark, 80);
    EXPECT_DOUBLE_EQ(m.finalMark, 80);
    EXPECT_EQ(m.feedback, "Good work");

    vector<AuditLog> h = auditService.history(s.submissionId);
    ASSERT_EQ(h.size(), 1u);
    EXPECT_EQ(h[0].actorId, lecturerId);
    EXPECT_EQ(h[0].reason, "Initial marking");
    EXPECT_EQ(h[0].oldValue, "status=Submitted, mark=-");
    EXPECT_EQ(h[0].newValue, "status=Marked, mark=80.00");
}

TEST_F(LmsH, Grade_LatePenaltyIsAppliedAutomatically) {
    setNow(86400 + 1);                                   // 2 days late
    Submission a = submit(studentId);
    setNow(3 * 86400);                                   // 3 days late
    Submission b = submit(student2Id);
    submissionService.gradeSubmission(lecturerId, a.submissionId, 80, "ok", "");
    submissionService.gradeSubmission(lecturerId, b.submissionId, 80, "ok", "");
    EXPECT_DOUBLE_EQ(subRepo.findById(a.submissionId).finalMark, 64.0);   // 80 less 20%
    EXPECT_DOUBLE_EQ(subRepo.findById(b.submissionId).finalMark, 56.0);   // 80 less 30%
    EXPECT_DOUBLE_EQ(subRepo.findById(a.submissionId).rawMark, 80.0);     // raw mark kept
}

TEST_F(LmsH, Grade_RoundsToTwoDecimals) {
    setNow(1);                                           // 1 day late = 10%
    Submission a = submit(studentId);
    Submission b = submit(student2Id);
    submissionService.gradeSubmission(lecturerId, a.submissionId, 77.77, "ok", "");
    submissionService.gradeSubmission(lecturerId, b.submissionId, 33.33, "ok", "");
    EXPECT_NEAR(subRepo.findById(a.submissionId).finalMark, 69.99, 1e-9);
    EXPECT_NEAR(subRepo.findById(b.submissionId).finalMark, 30.00, 1e-9);
}

TEST_F(LmsH, Grade_MarkBoundaries) {
    Submission s = submit(studentId);
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, -0.01, "fb", ""), runtime_error);
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 100.01, "fb", ""), runtime_error);
    EXPECT_NO_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 0, "fb", ""));       // lowest
    EXPECT_NO_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 100, "fb", "raised")); // highest
    EXPECT_DOUBLE_EQ(subRepo.findById(s.submissionId).finalMark, 100);
}

TEST_F(LmsH, Grade_ChangingAMarkNeedsAReasonAndIsAuditedTwice) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 85, "Better", ""), runtime_error);
    EXPECT_DOUBLE_EQ(subRepo.findById(s.submissionId).finalMark, 80);        // unchanged
    submissionService.gradeSubmission(lecturerId, s.submissionId, 85, "Better", "Re-checked the rubric");

    vector<AuditLog> h = auditService.history(s.submissionId);
    ASSERT_EQ(h.size(), 2u);
    EXPECT_EQ(h[1].oldValue, "status=Marked, mark=80.00");
    EXPECT_EQ(h[1].newValue, "status=Marked, mark=85.00");
    EXPECT_EQ(h[1].reason, "Re-checked the rubric");
}

TEST_F(LmsH, Grade_RejectsInvalidRequests) {
    Submission s = submit(studentId);
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 50, "", ""), runtime_error);      // no feedback
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 50, "   ", ""), runtime_error);
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, 9999, 50, "fb", ""), runtime_error);              // unknown id
    EXPECT_TRUE(auditService.history(s.submissionId).empty());                                                   // nothing logged
}

TEST_F(LmsH, Grade_CannotMarkAfterModerationOrRelease) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    moderationService.adjustMark(hodId, s.submissionId, 75, "Normalising");
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 50, "fb", "why"), runtime_error);
    moderationService.approveAndRelease(hodId, s.submissionId);
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 50, "fb", "why"), runtime_error);
}

TEST_F(LmsH, Grade_IsAtomicWithTheAuditLog) {
    Submission s = submit(studentId);
    db.execute("DROP TABLE audit_logs");                  // make the audit step fail on purpose
    EXPECT_THROW(submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "fb", ""), runtime_error);
    Submission after = subRepo.findById(s.submissionId);
    EXPECT_EQ(after.status, SUBMITTED);                   // the mark was rolled back, not half-saved
    EXPECT_DOUBLE_EQ(after.rawMark, -1);
}

// ModerationService (Moderate and Release use case, FR-06, FR-07) 
TEST_F(LmsH, Moderation_PendingListHasOnlyMarkedOrModeratedWork) {
    Submission s1 = submit(studentId);                    // stays Submitted
    Submission s2 = submit(student2Id);
    submissionService.gradeSubmission(lecturerId, s2.submissionId, 70, "ok", "");
    EXPECT_EQ(moderationService.getPendingMarks(deptId).size(), 1u);
    moderationService.adjustMark(hodId, s2.submissionId, 65, "moderated");
    EXPECT_EQ(moderationService.getPendingMarks(deptId).size(), 1u);      // Moderated is still pending
    moderationService.approveAndRelease(hodId, s2.submissionId);
    EXPECT_EQ(moderationService.getPendingMarks(deptId).size(), 0u);      // Released leaves the list
    EXPECT_EQ(subRepo.findById(s1.submissionId).status, SUBMITTED);
}

TEST_F(LmsH, Moderation_PendingListExcludesOtherDepartments) {
    Department d2;
    d2.departmentName = "Mathematics";
    courseRepo.saveDepartment(d2);
    Course c2;
    c2.courseCode = "MA1000";
    c2.courseName = "Calculus";
    c2.departmentId = d2.departmentId;
    courseRepo.save(c2);
    Enrollment e;
    e.studentId = studentId;
    e.courseId = c2.courseId;
    courseRepo.enroll(e);
    Assignment a2;
    a2.courseId = c2.courseId;
    a2.title = "M1";
    a2.description = "d";
    a2.deadline = secondsToText(due);
    a2.maxMark = 50;
    assignRepo.save(a2);

    Submission other;
    other.assignmentId = a2.assignmentId;
    other.studentId = studentId;
    other.filePath = uploadFile;
    submissionService.uploadSubmission(other);
    submissionService.gradeSubmission(lecturerId, other.submissionId, 40, "ok", "");

    EXPECT_EQ(moderationService.getPendingMarks(d2.departmentId).size(), 1u);
    EXPECT_EQ(moderationService.getPendingMarks(deptId).size(), 0u);
}

TEST_F(LmsH, Moderation_AdjustMarkIsAuditedWithReasoning) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    moderationService.adjustMark(hodId, s.submissionId, 75, "Normalising marks");

    Submission m = subRepo.findById(s.submissionId);
    EXPECT_EQ(m.status, MODERATED);
    EXPECT_DOUBLE_EQ(m.finalMark, 75);
    EXPECT_DOUBLE_EQ(m.rawMark, 80);

    vector<AuditLog> h = auditService.history(s.submissionId);
    ASSERT_EQ(h.size(), 2u);
    EXPECT_EQ(h[1].actorId, hodId);
    EXPECT_EQ(h[1].oldValue, "status=Marked, mark=80.00");
    EXPECT_EQ(h[1].newValue, "status=Moderated, mark=75.00");
    EXPECT_EQ(h[1].reason, "Normalising marks");
}

TEST_F(LmsH, Moderation_AdjustMarkRules) {
    Submission s = submit(studentId);
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, 50, "why"), runtime_error);    // not marked yet
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, 50, ""), runtime_error);       // no reasoning
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, 100.01, "why"), runtime_error);
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, -1, "why"), runtime_error);
    EXPECT_THROW(moderationService.adjustMark(hodId, 9999, 50, "why"), runtime_error);
    EXPECT_NO_THROW(moderationService.adjustMark(hodId, s.submissionId, 0, "lowest"));
    EXPECT_NO_THROW(moderationService.adjustMark(hodId, s.submissionId, 100, "highest"));
    moderationService.approveAndRelease(hodId, s.submissionId);
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, 50, "why"), runtime_error);    // released
}

TEST_F(LmsH, Moderation_ReleaseMakesTheMarkOfficialAndIsAudited) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    moderationService.approveAndRelease(hodId, s.submissionId);
    EXPECT_EQ(subRepo.findById(s.submissionId).status, RELEASED);

    vector<AuditLog> h = auditService.history(s.submissionId);
    ASSERT_EQ(h.size(), 2u);
    EXPECT_EQ(h[1].actorId, hodId);
    EXPECT_EQ(h[1].newValue, "status=Released, mark=80.00");
    EXPECT_EQ(h[1].reason, "Approved for release by the Head of Department");
}

TEST_F(LmsH, Moderation_ReleaseRules) {
    Submission s = submit(studentId);
    EXPECT_THROW(moderationService.approveAndRelease(hodId, s.submissionId), runtime_error);   // not marked
    EXPECT_THROW(moderationService.approveAndRelease(hodId, 9999), runtime_error);             // unknown
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    EXPECT_NO_THROW(moderationService.approveAndRelease(hodId, s.submissionId));
    EXPECT_THROW(moderationService.approveAndRelease(hodId, s.submissionId), runtime_error);   // twice
}

TEST_F(LmsH, Moderation_ReleaseIsAtomicWithTheAuditLog) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    db.execute("DROP TABLE audit_logs");
    EXPECT_THROW(moderationService.approveAndRelease(hodId, s.submissionId), runtime_error);
    EXPECT_EQ(subRepo.findById(s.submissionId).status, MARKED);       // rolled back
}

TEST_F(LmsH, Moderation_AdjustIsAtomicWithTheAuditLog) {
    Submission s = submit(studentId);
    submissionService.gradeSubmission(lecturerId, s.submissionId, 80, "Good", "");
    db.execute("DROP TABLE audit_logs");
    EXPECT_THROW(moderationService.adjustMark(hodId, s.submissionId, 70, "why"), runtime_error);
    Submission after = subRepo.findById(s.submissionId);
    EXPECT_EQ(after.status, MARKED);
    EXPECT_DOUBLE_EQ(after.finalMark, 80);
}