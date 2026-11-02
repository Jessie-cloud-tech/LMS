#pragma once
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include "database.h"
#include "userRepo.h"
#include "courseRepo.h"
#include "assignRepo.h"
#include "submissionRepo.h"
#include "auditlogRepo.h"
#include "standardLinearPenaltyStrategy.h"
#include "auditservice.h"
#include "authservice.h"
#include "submissionservice.h"
#include "moderationservice.h"
#include "datetime.h"
using namespace std;

// Every test starts with a brand-new in-memory database that contains:
// one department, one course, a lecturer, an HoD, two enrolled students, one student who is
// NOT enrolled ("outsider"), and one assignment due on 10/06/2026 12:00.
// The fake clock starts one hour before the deadline.
class LmsH : public ::testing::Test {
protected:
    Database db;
    UserRepository userRepo;
    CourseRepository courseRepo;
    AssignRepository assignRepo;
    SubmissionRepo subRepo;
    AuditLogRepository auditRepo;
    StandardLinearPenaltyStrategy penalty;
    AuditService auditService;
    AuthService authService;
    SubmissionService submissionService;
    ModerationService moderationService;

    int deptId = 0, courseId = 0, lecturerId = 0, hodId = 0;
    int studentId = 0, student2Id = 0, outsiderId = 0, assignmentId = 0;
    long long due = 0;                         // the assignment deadline, in seconds
    const string uploadFile = "lms_test_upload.txt";

    LmsH()
        : db(":memory:"), userRepo(&db), courseRepo(&db), assignRepo(&db), subRepo(&db),
          auditRepo(&db), auditService(&auditRepo), authService(&userRepo),
          submissionService(&subRepo, &assignRepo, &courseRepo, &penalty, &auditService, &db),
          moderationService(&subRepo, &assignRepo, &auditService, &db) {}

    void SetUp() override {
        db.createTables();

        Department d;
        d.departmentName = "Computer Science";
        courseRepo.saveDepartment(d);
        deptId = d.departmentId;

        Course c;
        c.courseCode = "CS4433";
        c.courseName = "Software Engineering";
        c.departmentId = deptId;
        courseRepo.save(c);
        courseId = c.courseId;

        lecturerId = addUser("lect1", "Lecturer");
        hodId = addUser("hod1", "HoD");
        studentId = addUser("stu1", "Student");
        student2Id = addUser("stu2", "Student");
        outsiderId = addUser("outsider", "Student");

        Enrollment e1;
        e1.studentId = studentId;
        e1.courseId = courseId;
        courseRepo.enroll(e1);
        Enrollment e2;
        e2.studentId = student2Id;
        e2.courseId = courseId;
        courseRepo.enroll(e2);

        due = toSeconds("10/06/2026 12:00");
        Assignment a;
        a.courseId = courseId;
        a.title = "Assignment 1";
        a.description = "Write a report";
        a.deadline = secondsToText(due);
        a.maxMark = 100;
        assignRepo.save(a);
        assignmentId = a.assignmentId;

        ofstream(uploadFile.c_str()) << "student work";
        setTestTime(due - 3600);
    }

    void TearDown() override {
        std::remove(uploadFile.c_str());
        clearTestTime();
    }

    int addUser(string name, string role) {
        UserRecord u;
        u.username = name;
        u.passwordHash = "not-a-real-hash";     // fast: only the Auth tests need real hashes
        u.role = role;
        u.departmentId = deptId;
        userRepo.save(u);
        return u.userId;
    }

    // "now" = the deadline plus this many seconds (negative = before the deadline)
    void setNow(long long secondsFromDeadline) { setTestTime(due + secondsFromDeadline); }

    Submission submit(int forStudent) {
        Submission s;
        s.assignmentId = assignmentId;
        s.studentId = forStudent;
        s.filePath = uploadFile;
        submissionService.uploadSubmission(s);
        return s;
    }
};