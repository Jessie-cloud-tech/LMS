#include "auditlogRepo.h"
using namespace std;

static string COLUMNS = "log_id, submission_id, actor_id, logged_at, old_value, new_value, reason";

static AuditLog rowToLog(vector<string> row) {
    AuditLog a;
    a.logId = stoi(row[0]);
    a.submissionId = stoi(row[1]);
    a.actorId = stoi(row[2]);
    a.timestamp = row[3];
    a.oldValue = row[4];
    a.newValue = row[5];
    a.reason = row[6];
    return a;
}

AuditLogRepository::AuditLogRepository(Database* database) { db = database; }

AuditLog AuditLogRepository::findById(int id) {
    vector<vector<string> > rows = db->query("SELECT " + COLUMNS + " FROM audit_logs WHERE log_id = ?", {to_string(id)});
    if (rows.empty()) return AuditLog();
    return rowToLog(rows[0]);
}

bool AuditLogRepository::save(AuditLog& a) {
    if (a.logId != 0) return false;      // old entries can never be changed
    db->execute("INSERT INTO audit_logs (submission_id, actor_id, logged_at, old_value, new_value, reason) "
                "VALUES (?, ?, ?, ?, ?, ?)",
                {to_string(a.submissionId), to_string(a.actorId), a.timestamp, a.oldValue, a.newValue, a.reason});
    a.logId = db->lastInsertId();
    return true;
}

bool AuditLogRepository::remove(int) { return false; }

vector<AuditLog> AuditLogRepository::listBySubmission(int submissionId) {
    vector<vector<string> > rows = db->query(
        "SELECT " + COLUMNS + " FROM audit_logs WHERE submission_id = ? ORDER BY log_id", {to_string(submissionId)});
    vector<AuditLog> list;
    for (vector<string> row : rows) list.push_back(rowToLog(row));
    return list;
}