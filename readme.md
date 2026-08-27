# Academic System Portal

A lightweight **Academic Management System written in C**, built around a simple console interface and text-file persistence. The project demonstrates practical use of structures, arrays, functions, file handling, validation, sorting, and role-based menus.

[![Build](https://github.com/mdrafsanjanee/Academic-Portal-System/actions/workflows/build.yml/badge.svg)](https://github.com/mdrafsanjanee/Academic-Portal-System/actions/workflows/build.yml)

## Why this project?

The goal is not to hide the implementation behind a framework. It is a deliberately simple C application organized like a small real-world software project: source code is separated from application data, the release is reproducible, and users can download a ready-to-run Windows package.

## Features

### Faculty
- Student record creation, update, search and deletion
- Attendance management
- Exam mark entry and automatic grade assignment
- Student record sorting by ID or marks
- Faculty ID-based access
- Persistent text-file storage

### Students
- Personal academic record
- Attendance and CGPA overview
- SGPA calculator
- CGPA goal tracker
- Study timer
- Quiz system

## Project Structure

```text
Academic-Portal-System/
├── src/
│   └── AcademicSystemPortal.c       # Release build entry point
├── data/
│   ├── students.txt                 # Demo student data
│   ├── faculty.txt                  # Demo faculty data
│   ├── class_count.txt              # Attendance state
│   └── quiz.txt                     # Quiz data
├── .github/
│   └── workflows/
│       └── build.yml                # Automated Windows build/package
├── Backup/                          # Earlier development prototype
├── Editable items/                  # Editable project assets
├── Submitables/                     # Original academic submission
├── LICENSE
└── README.md
```

The original academic implementation is retained under `Submitables/` for transparency. The release entry point in `src/` wraps that implementation and makes the application's data location independent of the directory from which the executable is launched.

## Download & Run

### Windows release

The recommended way to use the application is to download the latest **Windows ZIP** from the repository's GitHub Releases page.

1. Download `AcademicSystemPortal-Windows.zip`.
2. Extract the ZIP.
3. Open the extracted `AcademicSystemPortal` folder.
4. Run `AcademicSystemPortal.exe`.

No compiler or development environment is required for a release build.

### Demo accounts

The repository contains intentionally generic demo data:

| Role | ID |
|---|---:|
| Faculty | `9001` |
| Student | `1001` |

These are demonstration credentials only and are not intended as real authentication.

## Build from Source

A Windows machine with GCC/MinGW can build the application with:

```bash
gcc src/AcademicSystemPortal.c -std=c11 -Wall -Wextra -o AcademicSystemPortal.exe
```

The application creates/uses a `data` directory next to the executable and stores its text-file data there.

## Automated Build

Every push and pull request is checked by GitHub Actions. Version tags such as `v1.0.0` automatically produce a Windows ZIP release package.

The workflow builds the executable, packages the executable with its required data files, uploads the build artifact, and publishes the package to GitHub Releases for version tags.

## Data Model

The project intentionally uses plain text files instead of an external database so the implementation remains easy to understand and suitable for an introductory C project.

```text
students.txt
  ID Name Department Semester Attendance Marks Grade CGPA

faculty.txt
  ID Name Department

class_count.txt
  Total classes

quiz.txt
  Question|Option A|Option B|Option C|Option D|Correct option
```

The data directory is separate from the source code and executable, making it easier to back up, reset, or replace application data without modifying the program.

## Technologies

- C11-compatible C compiler
- Standard C file I/O
- Structures and arrays
- Functions and modular programming
- Windows API for application-directory handling and console timing
- GitHub Actions for automated builds and releases

## Project Context

Developed as a **CSE103** university project at **East West University** to apply fundamental C programming concepts to a practical academic management system.

The project is being maintained beyond the original coursework submission as a small software-engineering exercise focused on code organization, usability, reproducible builds, and distribution.

## Current Scope

This is a small-scale, local, single-user-at-a-time academic management application. It is not intended to provide production-grade authentication, concurrent database access, or network functionality.

Potential future upgrades include:

- SQLite-backed storage
- Stronger input validation
- Proper authentication and password hashing
- Backup/export tools
- Reports and analytics
- A graphical interface
- Multi-user/network support

## License

MIT License. See [LICENSE](LICENSE).
