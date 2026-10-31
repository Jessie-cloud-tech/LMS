#pragma once
#include <string>
using namespace std;

enum SubmissionStatus { SUBMITTED, LATE, MARKED, MODERATED, RELEASED };

string statusToString(SubmissionStatus status);
SubmissionStatus stringToStatus(string text);
string markToText(double mark);    

class Submission {
public:
    int submissionId;
    int assignmentId;
    int studentId;
    string filePath;
    string subDate;
    SubmissionStatus status;
    double rawMark;       
    double finalMark;     
    string feedback;
    int daysLate;
    Submission();
    string describe();    
    string summary();     
};