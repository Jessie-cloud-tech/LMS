PRAGMA foreign_keys = ON; 

CREATE TABLE IF NOT EXISTS departments (
    department_id   INTEGER PRIMARY KEY AUTOINCREMENT,
    department_name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS users (
    user_id       INTEGER PRIMARY KEY AUTOINCREMENT,
    username      TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,                 
    role          TEXT NOT NULL CHECK (role IN ('Admin','Lecturer','HoD','Student')),
    department_id INTEGER NOT NULL DEFAULT 0     
);

CREATE TABLE IF NOT EXISTS courses (
    course_id     INTEGER PRIMARY KEY AUTOINCREMENT,
    course_code   TEXT NOT NULL UNIQUE,         
    course_name   TEXT NOT NULL,
    department_id INTEGER NOT NULL REFERENCES departments(department_id)
);

CREATE TABLE IF NOT EXISTS enrollments (
    enrollment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id    INTEGER NOT NULL REFERENCES users(user_id),
    course_id     INTEGER NOT NULL REFERENCES courses(course_id),
    UNIQUE (student_id, course_id)
);

CREATE TABLE IF NOT EXISTS assignments (
    assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id     INTEGER NOT NULL REFERENCES courses(course_id),
    title         TEXT NOT NULL,
    description   TEXT NOT NULL,
    deadline      TEXT NOT NULL,                 
    max_mark      REAL NOT NULL CHECK (max_mark > 0)
);

CREATE TABLE IF NOT EXISTS extensions (
    extension_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id    INTEGER NOT NULL REFERENCES users(user_id),
    assignment_id INTEGER NOT NULL REFERENCES assignments(assignment_id),
    ext_deadline  TEXT NOT NULL,
    reason        TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS submissions (
    submission_id INTEGER PRIMARY KEY AUTOINCREMENT,
    assignment_id INTEGER NOT NULL REFERENCES assignments(assignment_id),
    student_id    INTEGER NOT NULL REFERENCES users(user_id),
    file_path     TEXT NOT NULL,                 
    sub_date      TEXT NOT NULL,
    status        TEXT NOT NULL CHECK (status IN
                  ('Submitted','Late','Marked','Moderated','Released','Rejected')),
    raw_mark      REAL NOT NULL DEFAULT -1,      
    final_mark    REAL NOT NULL DEFAULT -1,      
    feedback      TEXT NOT NULL DEFAULT '',
    days_late     INTEGER NOT NULL DEFAULT 0,
    UNIQUE (assignment_id, student_id)
);
 
CREATE TABLE IF NOT EXISTS audit_logs (
    log_id        INTEGER PRIMARY KEY AUTOINCREMENT,
    submission_id INTEGER NOT NULL REFERENCES submissions(submission_id),
    actor_id      INTEGER NOT NULL REFERENCES users(user_id),
    logged_at     TEXT NOT NULL,
    old_value     TEXT NOT NULL,
    new_value     TEXT NOT NULL,
    reason        TEXT NOT NULL CHECK (length(trim(reason)) > 0)
);

// Phase 8: Class-wide extensions
CREATE TABLE IF NOT EXISTS ClassExtensions (
    assignment_id INTEGER PRIMARY KEY,
    ext_deadline TEXT NOT NULL,
    reason TEXT NOT NULL,
    lecturer_id INTEGER NOT NULL,

    FOREIGN KEY (assignment_id)
        REFERENCES Assignments(assignment_id),

    FOREIGN KEY (lecturer_id)
        REFERENCES Users(user_id)
);