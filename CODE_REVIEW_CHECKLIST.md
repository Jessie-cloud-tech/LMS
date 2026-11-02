# LMS Self-Code Review Checklist

Date: 02 November 2026
Reviewer: Developer
Scope: LMS C++17 source code

## Code quality
- [ ] Functions have clear, meaningful names.
- [ ] Variables and classes use consistent naming.
- [ ] Unnecessary duplicated code has been reviewed.
- [ ] Functions have manageable responsibilities.
- [ ] Comments explain important or non-obvious logic.

## Functionality
- [ ] Input validation is applied.
- [ ] Exceptions are handled appropriately.
- [ ] Database operations use parameterised queries.
- [ ] Role-based access is enforced.
- [ ] Submission deadlines and extensions are enforced.
- [ ] Late penalties follow the approved rules.
- [ ] Mark changes are audited.
- [ ] Moderation and release rules are enforced.

## Security and reliability
- [ ] Passwords are not stored as plain text.
- [ ] Database errors are handled.
- [ ] Resources are managed safely.
- [ ] Sensitive information is not unnecessarily displayed.

## Testing and maintenance
- [ ] Automated tests pass.
- [ ] Static analysis findings have been reviewed.
- [ ] High-complexity functions have been reviewed.
- [ ] No temporary debugging code remains.

## Review outcome

Reviewer conclusion:
Reviews for correctness, readibility, maintainability, security and reliability have been made and no changes were necessary.