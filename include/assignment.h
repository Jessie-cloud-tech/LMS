#pragma once
#include <string>
using namespace std;

class Assignment {
public:
    int assignmentId;
    int courseId;
    string title;
    string description;
    string deadline;
    double maxMark;
    Assignment();
};