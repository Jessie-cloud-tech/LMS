#include "student.h"
#include "input.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Student::Student(UserRecord record, CourseRepository* courses, AssignRepository* assignments,
                 SubmissionRepo* submissions, SubmissionService* submissionSvc)
    : User(record) {
    courseRepo = courses;
    assignRepo = assignments;
    subRepo = submissions;
    submissionService = submissionSvc;
}

void Student::viewCourses() {
    vector<Course> list = courseRepo->listForStudent(userId);
    cout << "Your courses:\n";
    if (list.empty()) cout << "  (you are not enrolled in any course yet)\n";
    for (Course c : list) {
        cout << "  [" << c.courseId << "] " << c.courseCode << " - " << c.courseName << "\n";
    }
}

void Student::viewAssignments() {
    viewCourses();
    int courseId = readInt("Course id");
    if (!courseRepo->isEnrolled(userId, courseId)) throw runtime_error("You are not enrolled in that course.");
    vector<Assignment> list = assignRepo->listByCourse(courseId);
    if (list.empty()) cout << "  (no assignments yet)\n";
    for (Assignment a : list) {
        string due = a.deadline;
        Extension e = assignRepo->findExtension(a.assignmentId, userId);
        if (e.extensionId != 0) due = e.extDeadline + " (your extended deadline)";
        cout << "  [" << a.assignmentId << "] " << a.title << " | max mark " << a.maxMark
             << " | due " << due << "\n      " << a.description << "\n";
    }
}

void Student::sendFile(bool isResubmit) {
    int assignmentId = readInt("Assignment id");
    Submission old = subRepo->find(assignmentId, userId);
    bool alreadySubmitted = (old.submissionId != 0);
    if (!isResubmit && alreadySubmitted) throw runtime_error("You already submitted this. Choose 'Resubmit' to replace it.");
    if (isResubmit && !alreadySubmitted) throw runtime_error("You have not submitted this assignment yet.");
    if (isResubmit && !readYesNo("This replaces your old submission. Continue?")) {
        cout << "Cancelled.\n";
        return;
    }
    Submission sub;
    sub.assignmentId = assignmentId;
    sub.studentId = userId;
    sub.filePath = readLine("Path of the file to upload");
    cout << submissionService->uploadSubmission(sub) << "\n";
}

void Student::uploadWork() { sendFile(false); }
void Student::resubmit() { sendFile(true); }

void Student::viewGrades() {
    vector<Submission> list = subRepo->listByStudent(userId);
    if (list.empty()) cout << "  (you have not submitted anything yet)\n";
    for (Submission s : list) {
        cout << "  [" << s.submissionId << "] assignment " << s.assignmentId << " | "
             << statusToString(s.status) << " | submitted " << s.subDate;
        if (s.status == RELEASED) {
            cout << " | mark " << markToText(s.finalMark) << " | feedback: " << s.feedback;
        } else {
            cout << " | result not released yet";    
        }
        cout << "\n";
    }
}

void Student::menu() {
    while (true) {
        cout << "\n Student: " << username << " \n";
        cout << "1) My courses\n";
        cout << "2) Assignments in a course\n";
        cout << "3) Submit work\n";
        cout << "4) Resubmit work\n";
        cout << "5) My grades\n";
        cout << "0) Logout\n";
        int choice = readInt("Choose");
        if (choice == 0) return;
        try {
            if (choice == 1) viewCourses();
            else if (choice == 2) viewAssignments();
            else if (choice == 3) uploadWork();
            else if (choice == 4) resubmit();
            else if (choice == 5) viewGrades();
            else cout << "Invalid choice.\n";
        } catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}