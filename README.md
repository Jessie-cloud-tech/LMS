# LMS
Moderated Learning Management System
INCLUDE
#contains header files
admin.h
auditlog.h
auditlogRepo.h
auditservice.h
authservice.h
database.h
hod.h
ipenaltyStrategy.h
irepository.h
lecturer.h
moderationservice.h
password.h
standardLinearPenaltyStrategy.h
student.h
submission.h
submissionRepo.h
submissionservice.h
user.h
userRepo.h
course.h
extension.h
department.h
enrollment.h
input.h
courseRepo.h
assignment.h
assignRepo.h
userrecord.h

SRC
#contains cpp files
admin.cpp
auditlog.cpp
auditlogRepo.cpp
auditservice.cpp
authservice.cpp
database.cpp
hod.cpp
ipenaltyStrategy.cpp
irepository.cpp
lecturer.cpp
moderationservice.cpp
password.cpp
standardLinearPenaltyStrategy.cpp
student.cpp
submission.cpp
submissionRepo.cpp
submissionservice.cpp
user.cpp
userRepo.cpp
course.cpp
extension.cpp
department.cpp
enrollment.cpp
input.cpp
courseRepo.cpp
assignment.cpp
assignRepo.cpp
userrecord.cpp

CMake
cMakeLists.txt

Compilation and building
g++ -std=c++17 -Wall -Wextra -Iinclude (Get-ChildItem src\*.cpp).FullName main.cpp -o lms.exe -lsqlite3 -lsodium
.\lms.exe
cmake --build build
.\build\LMS.exe
