#pragma once
#include "user.h"
#include "courseRepo.h"
#include "assignRepo.h"
#include "submissionRepo.h"
#include "submissionservice.h"

class Lecturer : public User {
private:
    CourseRepository* courseRepo;
    AssignRepository* assignRepo;
    SubmissionRepo* subRepo;
    SubmissionService* submissionService;

    void viewCourses();
    void viewSubmissions();
public:
    Lecturer(UserRecord record, CourseRepository* courses, AssignRepository* assignments,
             SubmissionRepo* submissions, SubmissionService* submissionSvc);
    void createAssign();    
    void grantExt();        
    void gradeWork();       
    virtual void menu();
};