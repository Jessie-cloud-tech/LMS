#pragma once
#include <string>
using namespace std;

class Extension {
public:
    int extensionId;
    int studentId;
    int assignmentId;
    string extDeadline;    
    string reason;
    Extension();
};