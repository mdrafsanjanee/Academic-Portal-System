# Academic System Portal

A console-based **Academic Management System** developed in C as a university CSE project.

The Academic System Portal is designed to simplify common academic management tasks such as student records, faculty information, attendance, marks, grades, and quizzes through a menu-driven console interface.

## Features

* Student record management
* Faculty information management
* Student attendance management
* Marks and grade management
* Quiz management
* Student search and record viewing
* Persistent data storage using text files
* Console-based menu navigation
* Separate access areas for different system functions

## Technologies Used

* **C**
* **File Handling**
* **Structures**
* **Arrays**
* **Functions**
* **Conditional Statements & Loops**
* **Text-based Database Storage**

## Project Structure

```text
Academic-Portal-System/
│
├── Backup/
│   └── Prototype of System Portal.c
│
├── Editable items/
│   └── AcademicPortalFlowchart.drawio
│
├── Submitables/
│   ├── AcademicSystemPortal.c
│   ├── AcademicPortalFlowchart.drawio
│   ├── Presentation of Academic System Portal.pptx
│   ├── Project Report of Academic System Portal.pdf
│   ├── classCount.txt
│   ├── facultyDB.txt
│   └── stdDB.txt
│
├── LICENSE
└── README.md
```

> The project structure may change as the application is prepared for standalone distribution.

## Getting Started

### Running from Source

To compile and run the project from source, you need a C compiler such as **GCC**.

Using GCC:

```bash
gcc "Submitables/AcademicSystemPortal.c" -o AcademicSystemPortal
```

Then run:

### Windows

```bash
AcademicSystemPortal.exe
```

## Data Storage

The current version uses text files for persistent data storage.

The main database files include:

```text
stdDB.txt
facultyDB.txt
classCount.txt
```

The database system is currently file-based and intended for a small-scale academic management environment.

> **Note:** The database/storage system is planned to be improved in future versions. The application will eventually use a dedicated data directory so that database files are handled independently from the executable.

## Download

A standalone Windows executable will be provided through **GitHub Releases** once the application has been packaged and tested.

The planned distribution will allow users to:

1. Download the release package.
2. Extract the files.
3. Run `AcademicSystemPortal.exe`.

No development environment will be required to run the released version.

## Documentation

The repository includes supporting project materials:

* **Project Report** — Detailed documentation of the system and development process.
* **Presentation** — Project presentation slides.
* **Flowchart** — Visual representation of the system's workflow.
* **Source Code** — Complete C implementation.

## Limitations

The current version has several limitations:

* Uses text files instead of a dedicated database system.
* Console-based user interface.
* Designed primarily for a small-scale academic environment.
* Limited data validation and security compared with production systems.
* The current data storage mechanism depends on the application's file paths.
* No network or multi-user functionality.

## Future Scope

Possible improvements include:

* Dedicated database integration such as SQLite or MySQL.
* Improved data validation and error handling.
* A graphical user interface.
* User authentication and role-based access control.
* Improved database security.
* Automated backup and recovery.
* Network-based multi-user functionality.
* Improved reporting and analytics.
* Standalone Windows installer.
* More robust application data management.

## Academic Context

This project was developed as part of the **CSE103** coursework at **East West University**.

The project was created to apply fundamental C programming concepts to a practical academic management scenario, particularly:

* Structures
* Functions
* Arrays
* File handling
* Input/output
* Control flow
* Modular programming

## Contributors

Developed as a group academic project.

MD. Rafsan Janee.
[Github](https://github.com/mdrafsanjanee).

Department of Computer Science and Engineering.

## License

This project is licensed under the **MIT License**.