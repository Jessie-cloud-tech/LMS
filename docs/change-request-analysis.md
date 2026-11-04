# Phase 8 – Change Request Analysis

## CR1
A lecturer should be able to extend the deadline for a whole class,
not just an individual student, and extensions should be shown on
the student's assignment list.

## 1. Maintenance classification

**Perfective maintenance**

Perfective maintenance improves existing software functionality
to meet changing user requirements.

The LMS already supports individual assignment extensions.
CR1 enhances this functionality by allowing lecturers to grant
extensions to an entire class and allowing students to see the
extended deadline on their assignment list.

It is not corrective maintenance because it does not fix an
existing defect. It is not adaptive maintenance because it does
not respond to a change in the operating environment.

## 2. Impact analysis

| Area                             | Impact                             
|----------------------------------|----------------------------------------------------------------------
| Lecturer requirements            | Add the ability to grant a class-wide extension. 
| Individual extension requirement | Preserve existing individual extensions. 
| Student requirements             | Display the effective deadline and extension type. 
| Submission requirement           | Check submissions against the effective deadline. 
| Use-case diagram                 | Add "Grant class-wide extension" and update "View assignments". 
| Class diagram                    | Update Lecturer, Assignment Repository, Student and SubmissionService. 
| Sequence diagram                 | Update the lecturer extension and student assignment-viewing workflows.
| Database                         | Add a class_extensions table. Keep the existing extensions table unchanged.
| Tests                            | Add tests for class extensions, deadline precedence and student display.

## 3. Deadline precedence

1. Individual extension, if one exists.
2. Class-wide extension, if one exists.
3. Original assignment deadline, if neither exists.

## 4. Affected test cases

- Lecturer can grant a class-wide extension.
- Invalid class-wide deadlines are rejected.
- Empty extension reasons are rejected.
- All enrolled students receive the class-wide extension.
- Individual extensions take precedence over class-wide extensions.
- Original deadline is used when no extension exists.
- Student assignment list displays the effective deadline.
- Submission and late-penalty calculations use the effective deadline.
- Existing individual extension functionality still works.