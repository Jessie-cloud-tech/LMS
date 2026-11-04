# LMS Release v1.1

**Change request:** CR1 – Class-wide assignment deadline extensions

## Changes implemented

Release v1.1 introduces class-wide assignment deadline extensions.

The lecturer can now:
- Grant an individual student an extension.
- Grant an extension to the whole class.

The student assignment list displays:
- Original assignment deadline.
- Effective assignment deadline.
- Extension type.

### Implementation changes

| File                  | Change 
|-----------------------|-----------------------------------------
| lecturer.h            | Declare grantClassExtension()
| lecturer.cpp          | Implement class-wide extension functionality and update lecturer menu.
| assignRepository.h    | Declare class extension database functions. 
| assignRepository.cpp  | Save and retrieve class-wide extensions. 
| database.cpp          | Create ClassExtensions table. 
| submissionservice.h   | Declare effective-deadline function. 
| submissionservice.cpp | Apply deadline precedence and use effective deadlines. 
| student.cpp           | Display effective deadlines and extension types. 

### Deadline precedence

Individual extension > Class-wide extension > Original deadline.

### Compatibility

Existing individual extension functionality and existing
assignment records are preserved.