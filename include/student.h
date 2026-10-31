#pragma once
#include "user.h"
#include "courseRepo.h"
#include "assignRepo.h"
#include "submissionRepo.h"
#include "submissionservice.h"

class Student : public User {
private:
    CourseRepository* courseRepo;
    AssignRepository* assignRepo;
    SubmissionRepo* subRepo;
    SubmissionService* submissionService;

    void sendFile(bool isResubmit);
public:
    Student(UserRecord record, CourseRepository* courses, AssignRepository* assignments,
            SubmissionRepo* submissions, SubmissionService* submissionSvc);
    void viewCourses();
    void viewAssignments();
    void uploadWork();     
    void resubmit();       
    void viewGrades();
    virtual void menu();
};