#pragma once
#include <vector>
#include "irepository.h"
#include "database.h"
#include "course.h"
#include "department.h"
#include "enrollment.h"

class CourseRepository : public IRepository<Course> {
private:
    Database* db;
public:
    CourseRepository(Database* database);
    virtual Course findById(int id);        // courseId is 0 when nothing is found
    virtual bool save(Course& course);
    virtual bool remove(int id);
    vector<Course> listAll();
    vector<Course> listForStudent(int studentId);
    bool saveDepartment(Department& department);
    vector<Department> listDepartments();
    bool enroll(Enrollment& enrollment);
    bool isEnrolled(int studentId, int courseId);
};