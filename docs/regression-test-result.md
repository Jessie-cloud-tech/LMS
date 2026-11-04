# LMS v1.1 Regression Test Results

## New CR1 tests

| Test | Expected result | Result |
|----------------------------|------------------------------------------------------|---------|----------
| Grant class-wide extension | Extension is saved.                                  | Pending |
| Reject invalid deadline    | Request is rejected.                                 | Pending |
| Reject empty reason        | Request is rejected.                                 | Pending |
| Apply class extension      | Enrolled students receive the extended deadline.     | Pending |
| Individual precedence      | Individual deadline is used.                         | Pending |
| No extension               | Original deadline is used.                           | Pending |
| Student assignment list    | Effective deadline and extension type are displayed. | Pending |
| Submission before deadline | Submission is accepted as on time.                   | Pending |
| Submission after deadline  | Late penalty is calculated correctly                 | Pending |

## Existing regression tests

The existing test suite must be rerun to verify:

- Authentication and role-based access.
- User and course management.
- Assignment creation.
- Individual extensions.
- Assignment submissions and resubmissions.
- Late submission penalties.
- Marking and feedback.
- HoD moderation.
- Official mark release.
- Audit logging.

## Test execution

Build:

    cmake --build build

Run all tests:

    ctest --test-dir build --output-on-failure

## Results

| Test category             | Passed  | Failed  |
|---------------------------|---------|----------
| New CR1 tests             |         |         |
| Existing regression tests |         |         |
| Total                     |         |         |

## Conclusion
