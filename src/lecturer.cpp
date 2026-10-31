#include "lecturer.h"
#include "input.h"
#include "datetime.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Lecturer::Lecturer(UserRecord record, CourseRepository* courses, AssignRepository* assignments,
                   SubmissionRepo* submissions, SubmissionService* submissionSvc)
    : User(record) {
    courseRepo = courses;
    assignRepo = assignments;
    subRepo = submissions;
    submissionService = submissionSvc;
}

void Lecturer::viewCourses() {
    vector<Course> list = courseRepo->listAll();
    cout << "Courses:\n";
    if (list.empty()) cout << "  (none)\n";
    for (Course c : list) {
        cout << "  [" << c.courseId << "] " << c.courseCode << " - " << c.courseName << "\n";
    }
}

void Lecturer::createAssign() {
    viewCourses();
    Assignment a;
    a.courseId = readInt("Course id");
    Course c = courseRepo->findById(a.courseId);
    if (c.courseId == 0) throw runtime_error("Course not found.");
    a.title = readLine("Title");
    a.description = readLine("Description");
    string deadline = readLine("Deadline (YYYY-MM-DD HH:MM)");
    a.deadline = secondsToText(toSeconds(deadline));     // checks that the date is real
    a.maxMark = readDouble("Maximum mark");
    if (a.title == "" || a.description == "") throw runtime_error("Title and description cannot be empty.");
    if (a.maxMark <= 0) throw runtime_error("The maximum mark must be more than 0.");
    assignRepo->save(a);
    cout << "Assignment created with id " << a.assignmentId << ".\n";
}

void Lecturer::grantExt() {
    int assignmentId = readInt("Assignment id");
    Assignment a = assignRepo->findById(assignmentId);
    if (a.assignmentId == 0) throw runtime_error("Assignment not found.");

    Extension e;
    e.assignmentId = assignmentId;
    e.studentId = readInt("Student id");
    if (!courseRepo->isEnrolled(e.studentId, a.courseId)) {
        throw runtime_error("That student is not enrolled in this course.");
    }
    long long newTime = toSeconds(readLine("New deadline (YYYY-MM-DD HH:MM)"));
    if (newTime <= toSeconds(a.deadline)) throw runtime_error("The new deadline must be later than the old one.");
    e.extDeadline = secondsToText(newTime);
    e.reason = readLine("Reason for the extension");
    if (e.reason == "") throw runtime_error("A reason is required.");
    assignRepo->saveExtension(e);
    cout << "Extension saved.\n";
}

void Lecturer::viewSubmissions() {
    int assignmentId = readInt("Assignment id");
    vector<Submission> list = subRepo->listByAssignment(assignmentId);
    if (list.empty()) cout << "  (no submissions yet)\n";
    for (Submission s : list) cout << s.summary() << "\n";
}

void Lecturer::gradeWork() {
    int submissionId = readInt("Submission id");
    double mark = readDouble("Mark (before the late penalty)");
    string feedback = readLine("Written feedback");
    string reason = readLine("Reason for change (leave empty the first time you mark)");
    submissionService->gradeSubmission(userId, submissionId, mark, feedback, reason);
    cout << "Mark saved. The late penalty was applied and the change was logged.\n";
}

void Lecturer::menu() {
    while (true) {
        cout << "\n--- Lecturer: " << username << " ---\n";
        cout << "1) View courses\n";
        cout << "2) Create assignment\n";
        cout << "3) Give a student an extension\n";
        cout << "4) View submissions of an assignment\n";
        cout << "5) Mark a submission\n";
        cout << "0) Logout\n";
        int choice = readInt("Choose");
        if (choice == 0) return;
        try {
            if (choice == 1) viewCourses();
            else if (choice == 2) createAssign();
            else if (choice == 3) grantExt();
            else if (choice == 4) viewSubmissions();
            else if (choice == 5) gradeWork();
            else cout << "Invalid choice.\n";
        } catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}