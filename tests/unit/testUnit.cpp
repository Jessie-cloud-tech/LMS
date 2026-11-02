#include <gtest/gtest.h>
#include <stdexcept>
#include "standardLinearPenaltyStrategy.h"
#include "datetime.h"
#include "submission.h"
#include "auditlog.h"
#include "password.h"
#include "input.h"
#include "database.h"
using namespace std;

//  Penalty strategy: equivalence classes + boundaries
struct PenaltyCase {
    int days;
    double expected;
};
class PenaltyCases : public ::testing::TestWithParam<PenaltyCase> {};

TEST_P(PenaltyCases, PercentForDaysLate) {
    StandardLinearPenaltyStrategy p;
    EXPECT_DOUBLE_EQ(p.calculatePenalty(GetParam().days), GetParam().expected);
}

INSTANTIATE_TEST_SUITE_P(Boundaries, PenaltyCases,
    ::testing::Values(PenaltyCase{-1, 0},     // invalid class (before the deadline)
                      PenaltyCase{0, 0},      // boundary: on time
                      PenaltyCase{1, 10},     // boundary: first late day
                      PenaltyCase{2, 20},
                      PenaltyCase{3, 30},     // boundary: last accepted day
                      PenaltyCase{4, 100},    // boundary: window closed
                      PenaltyCase{1000, 100}));

TEST(PenaltyStrategy, WorksThroughTheInterface) {
    IPenaltyStrategy* strategy = new StandardLinearPenaltyStrategy();
    EXPECT_DOUBLE_EQ(strategy->calculatePenalty(2), 20);
    delete strategy;
}

// Date helper 
TEST(DateTime, ValidDatesConvertBothWays) {
    long long t = toSeconds("10/06/2026 12:00");
    EXPECT_EQ(secondsToText(t), "10/06/2026 12:00");
    EXPECT_EQ(toSeconds("10/06/2026 12:01") - t, 60);
    EXPECT_EQ(toSeconds("11/06/2026 12:00") - t, 86400);
    EXPECT_EQ(secondsToText(toSeconds("01/06/2026 9:5")), "01/06/2026 09:05");
}

TEST(DateTime, InvalidDatesAreRejected) {
    EXPECT_THROW(toSeconds("10-06-2026 12:00"), runtime_error);
    EXPECT_THROW(toSeconds("31/02/2026 12:00"), runtime_error);
    EXPECT_THROW(toSeconds("10/13/2026 12:00"), runtime_error);
    EXPECT_THROW(toSeconds("10/06/2026 25:00"), runtime_error);
    EXPECT_THROW(toSeconds("invalid date"), runtime_error);
}

TEST(DateTime, TestClockOverridesTheRealClock) {
    setTestTime(123456);
    EXPECT_EQ(nowSeconds(), 123456);
    EXPECT_EQ(nowText(), secondsToText(123456));
    clearTestTime();
    EXPECT_GT(nowSeconds(), 1700000000LL);      // the real clock is back
}

// Submission helpers 
TEST(SubmissionHelpers, StatusTextRoundTrip) {
    SubmissionStatus all[] = {SUBMITTED, LATE, MARKED, MODERATED, RELEASED};
    for (SubmissionStatus s : all) {
        EXPECT_EQ(stringToStatus(statusToString(s)), s);
    }
    EXPECT_EQ(statusToString(RELEASED), "Released");
    EXPECT_THROW(stringToStatus("Deleted"), runtime_error);
}

TEST(SubmissionHelpers, MarkText) {
    EXPECT_EQ(markToText(-1), "-");
    EXPECT_EQ(markToText(0), "0.00");
    EXPECT_EQ(markToText(72.5), "72.50");
}

TEST(SubmissionHelpers, DefaultsAndDescriptions) {
    Submission s;
    EXPECT_EQ(s.submissionId, 0);
    EXPECT_EQ(s.status, SUBMITTED);
    EXPECT_DOUBLE_EQ(s.rawMark, -1);
    EXPECT_EQ(s.describe(), "status=Submitted, mark=-");
    s.submissionId = 7;
    s.finalMark = 64;
    s.status = MARKED;
    EXPECT_EQ(s.describe(), "status=Marked, mark=64.00");
    EXPECT_NE(s.summary().find("[7]"), string::npos);
    EXPECT_NE(s.summary().find("final 64.00"), string::npos);
}

TEST(AuditLogModel, SummaryShowsAllFacts) {
    AuditLog a;
    a.logId = 3;
    a.actorId = 2;
    a.timestamp = "10/06/2026 12:00";
    a.oldValue = "old";
    a.newValue = "new";
    a.reason = "because";
    string text = a.summary();
    EXPECT_NE(text.find("#3"), string::npos);
    EXPECT_NE(text.find("user 2"), string::npos);
    EXPECT_NE(text.find("old  ->  new"), string::npos);
    EXPECT_NE(text.find("because"), string::npos);
}

//  Passwords 
TEST(Password, HashIsSaltedArgon2AndVerifies) {
    string h1 = makeHash("correct horse");
    string h2 = makeHash("correct horse");
    EXPECT_EQ(h1.rfind("$argon2", 0), 0u);
    EXPECT_EQ(h1.find("correct horse"), string::npos);   // never stored as plain text
    EXPECT_NE(h1, h2);                                   // different random salt each time
    EXPECT_TRUE(passwordMatches(h1, "correct horse"));
    EXPECT_FALSE(passwordMatches(h1, "correct horse "));
    EXPECT_FALSE(passwordMatches(h1, ""));
}

//  Input helper 
TEST(InputHelper, TrimRemovesSpacesAtBothEnds) {
    EXPECT_EQ(trim("  a b  "), "a b");
    EXPECT_EQ(trim("x"), "x");
    EXPECT_EQ(trim(""), "");
    EXPECT_EQ(trim("   "), "");
    EXPECT_EQ(trim("\t line \n"), "line");
}

//  SQL helper (Database) 
TEST(DatabaseHelper, OpeningAnImpossiblePathFails) {
    EXPECT_THROW(Database("/nonexistent_dir_12345/lms.db"), runtime_error);
}

TEST(DatabaseHelper, BadSqlThrowsInsteadOfCrashing) {
    Database db(":memory:");
    EXPECT_THROW(db.execute("THIS IS NOT SQL"), runtime_error);
    EXPECT_THROW(db.query("SELECT * FROM table_that_does_not_exist"), runtime_error);
}

TEST(DatabaseHelper, ParametersAreStoredLiterally) {
    Database db(":memory:");
    db.createTables();
    string nasty = "O'Brien'); DROP TABLE users;--";
    db.execute("INSERT INTO departments (department_name) VALUES (?)", {nasty});
    vector<vector<string> > rows = db.query("SELECT department_name FROM departments");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0][0], nasty);
    EXPECT_NO_THROW(db.query("SELECT COUNT(*) FROM users"));     // the table still exists
}

TEST(DatabaseHelper, TransactionCommitKeepsAndRollbackUndoes) {
    Database db(":memory:");
    db.createTables();
    db.begin();
    db.execute("INSERT INTO departments (department_name) VALUES (?)", {"Kept"});
    db.commit();
    db.begin();
    db.execute("INSERT INTO departments (department_name) VALUES (?)", {"Undone"});
    db.rollback();
    vector<vector<string> > rows = db.query("SELECT department_name FROM departments");
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0][0], "Kept");
}

TEST(DatabaseHelper, ForeignKeysAreEnforced) {
    Database db(":memory:");
    db.createTables();
    EXPECT_THROW(db.execute("INSERT INTO courses (course_code, course_name, department_id) VALUES (?, ?, ?)",
                            {"X1", "No such department", "999"}), runtime_error);
}