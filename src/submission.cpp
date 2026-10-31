#include "submission.h"
#include <cstdio>
#include <stdexcept>
using namespace std;

string statusToString(SubmissionStatus status) {
    if (status == SUBMITTED) return "Submitted";
    if (status == LATE) return "Late";
    if (status == MARKED) return "Marked";
    if (status == MODERATED) return "Moderated";
    return "Released";
}

SubmissionStatus stringToStatus(string text) {
    if (text == "Submitted") return SUBMITTED;
    if (text == "Late") return LATE;
    if (text == "Marked") return MARKED;
    if (text == "Moderated") return MODERATED;
    if (text == "Released") return RELEASED;
    throw runtime_error("Unknown status: " + text);
}

string markToText(double mark) {
    if (mark < 0) return "-";
    char buffer[32];
    snprintf(buffer, sizeof buffer, "%.2f", mark);
    return string(buffer);
}

Submission::Submission() {
    submissionId = 0;
    assignmentId = 0;
    studentId = 0;
    filePath = "";
    subDate = "";
    status = SUBMITTED;
    rawMark = -1;
    finalMark = -1;
    feedback = "";
    daysLate = 0;
}

string Submission::describe() {
    return "status=" + statusToString(status) + ", mark=" + markToText(finalMark);
}

string Submission::summary() {
    return "  [" + to_string(submissionId) + "] assignment " + to_string(assignmentId) +
           " | student " + to_string(studentId) + " | " + statusToString(status) +
           " | days late " + to_string(daysLate) + " | raw " + markToText(rawMark) +
           " | final " + markToText(finalMark);
}