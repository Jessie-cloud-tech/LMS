# Code Metrics Report

## 1. Project Information

- **Project:** Moderated Learning Management System (LMS)
- **Language:** C++17
- **Date:** 2 November 2026
- **Measurement scope:** `include/`, `src/`, `tests/` and `main.cpp`

## 2. Lines of Code 

Lines of code measure the size of the source code. It helps estimate project size and maintenance effort.

**Tool:** cloc

**Command used:**
```powershell
cloc include src tests main.cpp
```

| Metric       | Result|
|--------------|-------|
| Source files | 63    |
| Blank lines  | 368   |
| Comment lines| 49    |
| Code lines   | 2 645 |


## 3. Cyclomatic Complexity

Cyclomatic complexity measures the number of independent execution paths through a function.

A higher value may indicate that a function is more difficult to understand, test and maintain.

**Tool:** lizard

**Measurement scope:** `src/`
|Function	                              | File	                                      |Complexity
|-----------------------------------------|-----------------------------------------------|---------------------------
|toSeconds()	                          |datetime.cpp	                                  |18
|Admin::menu()	                          |admin.cpp	                                  |13
|SubmissionService::gradeSubmission()	  |submissionservice.cpp	                      |12
|SubmissionService::uploadSubmission()	  |submissionservice.cpp	                      |11
|Lecturer::menu()	                      |lecturer.cpp	                                  |9
|Student::menu()	                      |student.cpp	                                  |9
|HoD::menu()	                          |hod.cpp	                                      |8
|ModerationService::adjustMark()	      |moderationservice.cpp	                      |8
|checkUsername()	                      |authservice.cpp	                              |7
|Student::sendFile()	                  |student.cpp	                                  |7
|readDouble()	                          |input.cpp	                                  |6
|stringToStatus()	                      |submission.cpp	                              |6
|chooseRole()	                          |admin.cpp	                                  |5
|trim()	                                  |input.cpp	                                  |5
|ModerationService::approveAndRelease()	  |moderationservice.cpp	                      |5
|Lecturer::createAssign()	              |lecturer.cpp	                                  |5
|Lecturer::grantExt()	                  |lecturer.cpp	                                  |5
|statusToString()	                      |submission.cpp	                              |5
|Student::viewAssignments()	              |student.cpp                                 	  |5
|Admin::assignRole()	                  |admin.cpp	                                  |4
|Database::query()	                      |database.cpp	                                  |4
|readInt()	                              |input.cpp                                   	  |4
|Student::viewGrades()	                  |student.cpp	                                  |4
|SubmissionService::validateDeadline()	  |submissionservice.cpp	                      |4
|Admin::showDepartments()	              |admin.cpp	                                  |3
|Admin::createCourse()	                  |admin.cpp	                                  |3
|Admin::enrolStudent()	                  |admin.cpp	                                  |3
|Admin::viewAuditLog()	                  |admin.cpp	                                  |3
|checkPassword()	                      |authservice.cpp	                              |3
-from highest to lowest complexity
## 4. Code Coverage

Code coverage measures how much of the source code is executed by automated tests.

The following individual file coverage figures were recorded:

| Source file                       | Coverage   |
|-----------------------------------|-----------:|
| authservice.cpp                   | 78%        |
| auditservice.cpp                  | 77%        |
| submissionservice.cpp             | 89%        |
| moderationservice.cpp             | 85%        |
| standardLinearPenaltyStrategy.cpp | 86%        |
| ipenaltyStrategy.cpp              | 84%        |
| submission.cpp                    | 88%        |
| **Unweighted average**            | **83.86%** |

The unweighted average is the arithmetic mean of the seven file percentages. It is not necessarily the project's aggregate line coverage.

## 5. Interpretation

- Lines of code provides an indication of the size of the LMS project.
- Cyclomatic complexity helps identify functions that may require simplification or additional testing.
- Code coverage indicates how extensively automated tests exercise the measured source files.
- These metrics support maintainability, testing and quality assurance.

## 6. Conclusion

Code metrics were used to evaluate the size, complexity and test coverage of the LMS. The results help identify areas that may need additional testing, refactoring or maintenance.