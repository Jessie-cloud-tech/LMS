# # Static Analysis Report

## 1. Tool Information

- **Tool:** Cppcheck
- **Version:** 2.22.0
- **Programming language:** C++
- **C++ standard:** C++17
- **Analysis date:** 2 November 2026

## 2. Scope of Analysis

Cppcheck was used to analyse:

- `src/`
- `tests/`
- `main.cpp`

A total of 33 files were checked.

## 3. Analysis Configuration

The following checks were enabled:

- All available checks (`--enable=all`)
- Inconclusive findings (`--inconclusive`)
- Inline suppressions (`--inline-suppr`)
- C++17 standard
- Missing system include suppression

## 4. Results

Analysis completed successfully across all 33 files.

**Warning count:** 0

**Error count:** 0

**Style findings:** 58

**Performance findings:** 79

**Portability findings:** 0

## 5. Findings and Corrections
| Findings         | Action
|------------------|-----------
|Genuine errors    | Rectify source code
|Warnings          |fix where required
|False positives   |document and suppress
|Harmless findings |explain and document 

### Suppression Management

The project uses cppcheck-suppressions.txt to record
specific Cppcheck findings that have been reviewed
and intentionally suppressed.

Suppressions are only added when justified.
No warnings are suppressed merely to obtain a clean
static analysis report.

## 6. Conclusion

Cppcheck completed static analysis of the LMS source code and automated test files. The findings were reviewed to identify potential defects, maintainability concerns and performance issues. Corrections and remaining findings are documented above.