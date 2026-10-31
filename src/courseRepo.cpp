#include "courseRepo.h"
using namespace std;

static Course rowToCourse(vector<string> row) {
    Course c;
    c.courseId = stoi(row[0]);
    c.courseCode = row[1];
    c.courseName = row[2];
    c.departmentId = stoi(row[3]);
    return c;
}

CourseRepository::CourseRepository(Database* database) { db = database; }

Course CourseRepository::findById(int id) {
    vector<vector<string> > rows = db->query(
        "SELECT course_id, course_code, course_name, department_id FROM courses WHERE course_id = ?", {to_string(id)});
    if (rows.empty()) return Course();
    return rowToCourse(rows[0]);
}

bool CourseRepository::save(Course& c) {
    if (c.courseId == 0) {
        db->execute("INSERT INTO courses (course_code, course_name, department_id) VALUES (?, ?, ?)",
                    {c.courseCode, c.courseName, to_string(c.departmentId)});
        c.courseId = db->lastInsertId();
    } else {
        db->execute("UPDATE courses SET course_code = ?, course_name = ?, department_id = ? WHERE course_id = ?",
                    {c.courseCode, c.courseName, to_string(c.departmentId), to_string(c.courseId)});
    }
    return true;
}

bool CourseRepository::remove(int id) {
    db->execute("DELETE FROM courses WHERE course_id = ?", {to_string(id)});
    return true;
}

vector<Course> CourseRepository::listAll() {
    vector<vector<string> > rows = db->query(
        "SELECT course_id, course_code, course_name, department_id FROM courses ORDER BY course_id");
    vector<Course> list;
    for (vector<string> row : rows) list.push_back(rowToCourse(row));
    return list;
}

vector<Course> CourseRepository::listForStudent(int studentId) {
    vector<vector<string> > rows = db->query(
        "SELECT c.course_id, c.course_code, c.course_name, c.department_id "
        "FROM courses c JOIN enrollments e ON e.course_id = c.course_id "
        "WHERE e.student_id = ? ORDER BY c.course_id", {to_string(studentId)});
    vector<Course> list;
    for (vector<string> row : rows) list.push_back(rowToCourse(row));
    return list;
}

bool CourseRepository::saveDepartment(Department& d) {
    db->execute("INSERT INTO departments (department_name) VALUES (?)", {d.departmentName});
    d.departmentId = db->lastInsertId();
    return true;
}

vector<Department> CourseRepository::listDepartments() {
    vector<vector<string> > rows = db->query("SELECT department_id, department_name FROM departments ORDER BY department_id");
    vector<Department> list;
    for (vector<string> row : rows) {
        Department d;
        d.departmentId = stoi(row[0]);
        d.departmentName = row[1];
        list.push_back(d);
    }
    return list;
}

bool CourseRepository::enroll(Enrollment& e) {
    db->execute("INSERT INTO enrollments (student_id, course_id) VALUES (?, ?)",
                {to_string(e.studentId), to_string(e.courseId)});
    e.enrollmentId = db->lastInsertId();
    return true;
}

bool CourseRepository::isEnrolled(int studentId, int courseId) {
    vector<vector<string> > rows = db->query(
        "SELECT 1 FROM enrollments WHERE student_id = ? AND course_id = ?", {to_string(studentId), to_string(courseId)});
    return !rows.empty();
}