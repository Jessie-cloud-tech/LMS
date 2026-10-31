#include "hod.h"
#include "input.h"
#include <iostream>
#include <stdexcept>
using namespace std;

HoD::HoD(UserRecord record, ModerationService* moderation, AuditService* audit)
    : User(record) {
    moderationService = moderation;
    auditService = audit;
}

void HoD::reviewMarks() {
    vector<Submission> list = moderationService->getPendingMarks(departmentId);
    if (list.empty()) cout << "  (no marks waiting for approval)\n";
    for (Submission s : list) cout << s.summary() << "\n";
}

void HoD::adjustMark() {
    int submissionId = readInt("Submission id");
    double mark = readDouble("New mark");
    string reason = readLine("Written reasoning");
    moderationService->adjustMark(userId, submissionId, mark, reason);
    cout << "Mark adjusted and logged.\n";
}

void HoD::releaseMarks() {
    int id = readInt("Submission id to release (0 = release ALL waiting marks)");
    if (id != 0) {
        moderationService->approveAndRelease(userId, id);
        cout << "Released.\n";
        return;
    }
    vector<Submission> list = moderationService->getPendingMarks(departmentId);
    for (Submission s : list) {
        moderationService->approveAndRelease(userId, s.submissionId);
    }
    cout << list.size() << " mark(s) released.\n";
}

void HoD::viewAuditLog() {
    int id = readInt("Submission id");
    vector<AuditLog> logs = auditService->history(id);
    if (logs.empty()) cout << "  (no entries)\n";
    for (AuditLog log : logs) cout << log.summary() << "\n";
}

void HoD::menu() {
    while (true) {
        cout << "\n--- Head of Department: " << username << " ---\n";
        cout << "1) Review marks waiting for approval\n";
        cout << "2) Adjust a mark\n";
        cout << "3) Approve and release marks\n";
        cout << "4) View audit log of a submission\n";
        cout << "0) Logout\n";
        int choice = readInt("Choose");
        if (choice == 0) return;
        try {
            if (choice == 1) reviewMarks();
            else if (choice == 2) adjustMark();
            else if (choice == 3) releaseMarks();
            else if (choice == 4) viewAuditLog();
            else cout << "Invalid choice.\n";
        } catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}