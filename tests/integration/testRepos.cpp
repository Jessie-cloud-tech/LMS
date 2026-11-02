#include "lms.h"

//  UserRepository 
TEST_F(LmsH, UserRepo_SaveAndFind) {
    UserRecord u = userRepo.findByUsername("lect1");
    EXPECT_EQ(u.userId, lecturerId);
    EXPECT_EQ(u.role, "Lecturer");
    EXPECT_EQ(userRepo.findById(lecturerId).username, "lect1");
    EXPECT_EQ(userRepo.findByUsername("nobody").userId, 0);
    EXPECT_EQ(userRepo.findById(9999).userId, 0);
}

TEST_F(LmsH, UserRepo_DuplicateUsernameRejectedByTheDatabase) {
    UserRecord u;
    u.username = "lect1";
    u.passwordHash = "x";
    u.role = "Student";
    EXPECT_THROW(userRepo.save(u), runtime_error);
}

TEST_F(LmsH, UserRepo_InvalidRoleRejectedByTheDatabase) {
    UserRecord u;
    u.username = "hacker";
    u.passwordHash = "x";
    u.role = "SuperUser";
    EXPECT_THROW(userRepo.save(u), runtime_error);
}

TEST_F(LmsH, UserRepo_UpdateChangeRoleCountListRemove) {
    EXPECT_EQ(userRepo.countUsers(), 5);
    EXPECT_EQ(userRepo.listAll().size(), 5u);

    UserRecord u = userRepo.findById(studentId);
    u.passwordHash = "new-hash";
    userRepo.save(u);
    EXPECT_EQ(userRepo.findById(studentId).passwordHash, "new-hash");

    userRepo.changeRole(studentId, "Lecturer", deptId);
    EXPECT_EQ(userRepo.findById(studentId).role, "Lecturer");

    userRepo.remove(outsiderId);
    EXPECT_EQ(userRepo.findById(outsiderId).userId, 0);
    EXPECT_EQ(userRepo.countUsers(), 4);
}

TEST_F(LmsH, SqlInjectionAttemptsAreHarmless) {
    EXPECT_EQ(userRepo.findByUsername("' OR '1'='1").userId, 0);
    EXPECT_EQ(userRepo.findByUsername("x'; DROP TABLE users;--").userId, 0);
    EXPECT_EQ(userRepo.countUsers(), 5);
    EXPECT_THROW(authService.login("' OR '1'='1", "anything1"), runtime_error);
}

//  CourseRepository 
TEST_F(LmsH, CourseRepo_FindListAndEnrol) {
    EXPECT_EQ(courseRepo.findById(courseId).courseCode, "CS4433");
    EXPECT_EQ(courseRepo.findById(9999).courseId, 0);
    EXPECT_EQ(courseRepo.listAll().size(), 1u);
    EXPECT_EQ(courseRepo.listDepartments().size(), 1u);
    EXPECT_TRUE(courseRepo.isEnrolled(studentId, courseId));
    EXPECT_FALSE(courseRepo.isEnrolled(outsiderId, courseId));
    EXPECT_EQ(courseRepo.listForStudent(studentId).size(), 1u);
    EXPECT_EQ(courseRepo.listForStudent(outsiderId).size(), 0u);
}

TEST_F(LmsH, CourseRepo_DuplicateEnrolmentAndCourseCodeRejected) {
    Enrollment e;
    e.studentId = studentId;
    e.courseId = courseId;
    EXPECT_THROW(courseRepo.enroll(e), runtime_error);

    Course c;
    c.courseCode = "CS4433";           // same code again
    c.courseName = "Copy";
    c.departmentId = deptId;
    EXPECT_THROW(courseRepo.save(c), runtime_error);
}

TEST_F(LmsH, CourseRepo_UpdateAndRemove) {
    Course c = courseRepo.findById(courseId);
    c.courseName = "Renamed";
    courseRepo.save(c);
    EXPECT_EQ(courseRepo.findById(courseId).courseName, "Renamed");

    Course extra;
    extra.courseCode = "TMP1";
    extra.courseName = "Temporary";
    extra.departmentId = deptId;
    courseRepo.save(extra);
    courseRepo.remove(extra.courseId);
    EXPECT_EQ(courseRepo.findById(extra.courseId).courseId, 0);
}

//  AssignRepository 
TEST_F(LmsH, AssignRepo_SaveFindListUpdateRemove) {
    Assignment a = assignRepo.findById(assignmentId);
    EXPECT_EQ(a.title, "Assignment 1");
    EXPECT_DOUBLE_EQ(a.maxMark, 100);
    EXPECT_EQ(assignRepo.findById(9999).assignmentId, 0);
    EXPECT_EQ(assignRepo.listByCourse(courseId).size(), 1u);

    a.title = "Renamed";
    assignRepo.save(a);
    EXPECT_EQ(assignRepo.findById(assignmentId).title, "Renamed");

    Assignment b;
    b.courseId = courseId;
    b.title = "Temp";
    b.description = "d";
    b.deadline = "10/06/2026 12:00";
    b.maxMark = 10;
    assignRepo.save(b);
    assignRepo.remove(b.assignmentId);
    EXPECT_EQ(assignRepo.listByCourse(courseId).size(), 1u);
}

TEST_F(LmsH, AssignRepo_MaximumMarkMustBePositive) {
    Assignment a;
    a.courseId = courseId;
    a.title = "Bad";
    a.description = "d";
    a.deadline = "10/06/2026 12:00";
    a.maxMark = 0;                                // boundary: not allowed
    EXPECT_THROW(assignRepo.save(a), runtime_error);
}

TEST_F(LmsH, AssignRepo_OneExtensionPerStudentLatestWins) {
    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = studentId;
    e.extDeadline = "20/06/2026 12:00";
    e.reason = "Illness";
    assignRepo.saveExtension(e);
    e.extDeadline = "25/06/2026 12:00";
    e.reason = "More time needed";
    assignRepo.saveExtension(e);

    Extension found = assignRepo.findExtension(assignmentId, studentId);
    EXPECT_EQ(found.extDeadline, "25/06/2026 12:00");
    EXPECT_EQ(found.reason, "More time needed");
    EXPECT_EQ(assignRepo.findExtension(assignmentId, student2Id).extensionId, 0);
}

//  SubmissionRepo
TEST_F(LmsH, SubmissionRepo_SaveFindAndUpdate) {
    Submission s = submit(studentId);
    Submission found = subRepo.find(assignmentId, studentId);
    EXPECT_EQ(found.submissionId, s.submissionId);
    EXPECT_EQ(found.filePath, uploadFile);
    EXPECT_EQ(subRepo.findById(s.submissionId).studentId, studentId);
    EXPECT_EQ(subRepo.findById(9999).submissionId, 0);
    EXPECT_EQ(subRepo.listByAssignment(assignmentId).size(), 1u);
    EXPECT_EQ(subRepo.listByStudent(studentId).size(), 1u);
    EXPECT_EQ(subRepo.listByStudent(student2Id).size(), 0u);

    found.feedback = "changed";
    subRepo.save(found);
    EXPECT_EQ(subRepo.findById(s.submissionId).feedback, "changed");
}

TEST_F(LmsH, SubmissionRepo_OneRowPerStudentPerAssignment) {
    submit(studentId);
    Submission duplicate;
    duplicate.assignmentId = assignmentId;
    duplicate.studentId = studentId;
    duplicate.filePath = "x";
    duplicate.subDate = "d";
    EXPECT_THROW(subRepo.save(duplicate), runtime_error);
}

TEST_F(LmsH, SubmissionRepo_Remove) {
    Submission s = submit(studentId);
    subRepo.remove(s.submissionId);
    EXPECT_EQ(subRepo.findById(s.submissionId).submissionId, 0);
}

//  AuditLogRepository 
TEST_F(LmsH, AuditRepo_IsAppendOnlyInApplicationCode) {
    Submission s = submit(studentId);
    auditService.logAction(lecturerId, s.submissionId, "old", "new", "testing");
    vector<AuditLog> logs = auditRepo.listBySubmission(s.submissionId);
    ASSERT_EQ(logs.size(), 1u);

    EXPECT_FALSE(auditRepo.remove(logs[0].logId));       // entries can never be deleted
    AuditLog changed = logs[0];
    EXPECT_FALSE(auditRepo.save(changed));               // ...or edited
    EXPECT_EQ(auditRepo.findById(logs[0].logId).reason, "testing");
    EXPECT_EQ(auditRepo.findById(9999).logId, 0);
}

TEST_F(LmsH, AuditRepo_RejectsLogForMissingSubmission) {
    EXPECT_THROW(auditService.logAction(lecturerId, 9999, "a", "b", "reason"), runtime_error);
}