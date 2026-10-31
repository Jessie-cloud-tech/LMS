#include "submissionRepo.h"
using namespace std;

static string COLUMNS =
    "s.submission_id, s.assignment_id, s.student_id, s.file_path, s.sub_date, s.status, "
    "s.raw_mark, s.final_mark, s.feedback, s.days_late";

static Submission rowToSubmission(vector<string> row) {
    Submission s;
    s.submissionId = stoi(row[0]);
    s.assignmentId = stoi(row[1]);
    s.studentId = stoi(row[2]);
    s.filePath = row[3];
    s.subDate = row[4];
    s.status = stringToStatus(row[5]);
    s.rawMark = stod(row[6]);
    s.finalMark = stod(row[7]);
    s.feedback = row[8];
    s.daysLate = stoi(row[9]);
    return s;
}

static vector<Submission> rowsToList(vector<vector<string> > rows) {
    vector<Submission> list;
    for (vector<string> row : rows) list.push_back(rowToSubmission(row));
    return list;
}

SubmissionRepo::SubmissionRepo(Database* database) { db = database; }

Submission SubmissionRepo::findById(int id) {
    vector<vector<string> > rows = db->query(
        "SELECT " + COLUMNS + " FROM submissions s WHERE s.submission_id = ?", {to_string(id)});
    if (rows.empty()) return Submission();
    return rowToSubmission(rows[0]);
}

Submission SubmissionRepo::find(int assignmentId, int studentId) {
    vector<vector<string> > rows = db->query(
        "SELECT " + COLUMNS + " FROM submissions s WHERE s.assignment_id = ? AND s.student_id = ?",
        {to_string(assignmentId), to_string(studentId)});
    if (rows.empty()) return Submission();
    return rowToSubmission(rows[0]);
}

bool SubmissionRepo::save(Submission& s) {
    if (s.submissionId == 0) {
        db->execute("INSERT INTO submissions (assignment_id, student_id, file_path, sub_date, status, "
                    "raw_mark, final_mark, feedback, days_late) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
                    {to_string(s.assignmentId), to_string(s.studentId), s.filePath, s.subDate,
                     statusToString(s.status), to_string(s.rawMark), to_string(s.finalMark),
                     s.feedback, to_string(s.daysLate)});
        s.submissionId = db->lastInsertId();
    } else {
        db->execute("UPDATE submissions SET file_path = ?, sub_date = ?, status = ?, raw_mark = ?, "
                    "final_mark = ?, feedback = ?, days_late = ? WHERE submission_id = ?",
                    {s.filePath, s.subDate, statusToString(s.status), to_string(s.rawMark),
                     to_string(s.finalMark), s.feedback, to_string(s.daysLate), to_string(s.submissionId)});
    }
    return true;
}

bool SubmissionRepo::remove(int id) {
    db->execute("DELETE FROM submissions WHERE submission_id = ?", {to_string(id)});
    return true;
}

vector<Submission> SubmissionRepo::listByAssignment(int assignmentId) {
    return rowsToList(db->query(
        "SELECT " + COLUMNS + " FROM submissions s WHERE s.assignment_id = ? ORDER BY s.submission_id",
        {to_string(assignmentId)}));
}

vector<Submission> SubmissionRepo::listByStudent(int studentId) {
    return rowsToList(db->query(
        "SELECT " + COLUMNS + " FROM submissions s WHERE s.student_id = ? ORDER BY s.submission_id",
        {to_string(studentId)}));
}

vector<Submission> SubmissionRepo::listPendingByDepartment(int departmentId) {
    return rowsToList(db->query(
        "SELECT " + COLUMNS + " FROM submissions s "
        "JOIN assignments a ON a.assignment_id = s.assignment_id "
        "JOIN courses c ON c.course_id = a.course_id "
        "WHERE c.department_id = ? AND s.status IN ('Marked', 'Moderated') "
        "ORDER BY s.submission_id", {to_string(departmentId)}));
}