#include "assignRepo.h"
using namespace std;

static string COLUMNS = "assignment_id, course_id, title, description, deadline, max_mark";

static Assignment rowToAssignment(vector<string> row) {
    Assignment a;
    a.assignmentId = stoi(row[0]);
    a.courseId = stoi(row[1]);
    a.title = row[2];
    a.description = row[3];
    a.deadline = row[4];
    a.maxMark = stod(row[5]);
    return a;
}

AssignRepository::AssignRepository(Database* database) { db = database; }

Assignment AssignRepository::findById(int id) {
    vector<vector<string> > rows = db->query("SELECT " + COLUMNS + " FROM assignments WHERE assignment_id = ?", {to_string(id)});
    if (rows.empty()) return Assignment();
    return rowToAssignment(rows[0]);
}

bool AssignRepository::save(Assignment& a) {
    if (a.assignmentId == 0) {
        db->execute("INSERT INTO assignments (course_id, title, description, deadline, max_mark) VALUES (?, ?, ?, ?, ?)",
                    {to_string(a.courseId), a.title, a.description, a.deadline, to_string(a.maxMark)});
        a.assignmentId = db->lastInsertId();
    } else {
        db->execute("UPDATE assignments SET course_id = ?, title = ?, description = ?, deadline = ?, max_mark = ? WHERE assignment_id = ?",
                    {to_string(a.courseId), a.title, a.description, a.deadline, to_string(a.maxMark), to_string(a.assignmentId)});
    }
    return true;
}

bool AssignRepository::remove(int id) {
    db->execute("DELETE FROM assignments WHERE assignment_id = ?", {to_string(id)});
    return true;
}

vector<Assignment> AssignRepository::listByCourse(int courseId) {
    vector<vector<string> > rows = db->query(
        "SELECT " + COLUMNS + " FROM assignments WHERE course_id = ? ORDER BY assignment_id", {to_string(courseId)});
    vector<Assignment> list;
    for (vector<string> row : rows) list.push_back(rowToAssignment(row));
    return list;
}

bool AssignRepository::saveExtension(Extension& e) {
    // a student has only one extension per assignment, so remove the old one first
    db->execute("DELETE FROM extensions WHERE assignment_id = ? AND student_id = ?",
                {to_string(e.assignmentId), to_string(e.studentId)});
    db->execute("INSERT INTO extensions (student_id, assignment_id, ext_deadline, reason) VALUES (?, ?, ?, ?)",
                {to_string(e.studentId), to_string(e.assignmentId), e.extDeadline, e.reason});
    e.extensionId = db->lastInsertId();
    return true;
}

Extension AssignRepository::findExtension(int assignmentId, int studentId) {
    vector<vector<string> > rows = db->query(
        "SELECT extension_id, student_id, assignment_id, ext_deadline, reason FROM extensions "
        "WHERE assignment_id = ? AND student_id = ?", {to_string(assignmentId), to_string(studentId)});
    Extension e;
    if (rows.empty()) return e;
    e.extensionId = stoi(rows[0][0]);
    e.studentId = stoi(rows[0][1]);
    e.assignmentId = stoi(rows[0][2]);
    e.extDeadline = rows[0][3];
    e.reason = rows[0][4];
    return e;
}