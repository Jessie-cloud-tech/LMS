# Branching inn Git

## Main Branch
The main branch contains LMS files and, completed and successfl tests
Confirmed success functionalities are merged into this branch

## Develop branch
This branch is used for completed feature integration before official release

## Feature Branches
Feature branches are created from develop for individual features
or improvements.

Naming style:
feature/feature-name

## Bugfix Branches
Bugfix branches are created from develop to correct identified
software defects.

Naming style:
bugfix/bugDescription

## Pull Request Process
1. Create a feature or bugfix branch from develop.
2. Implement the required changes.
3. Commit changes using meaningful commit messages.
4. Push the branch to GitHub.
5. Open a pull request into develop.
6. Review the code and verify that automated tests pass.
7. Merge the approved changes iinto develop.
8. Merge tested release changes with the main branch.

## Releases
Stable versions are identified using Git tags.
First releae is called v1.0.0