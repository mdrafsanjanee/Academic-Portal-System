#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <direct.h>

#define MAX_STUDENTS 100
#define MAX_LINE 512
#define DATA_DIR "data\\"
#define STUDENT_DB DATA_DIR "stdDB.txt"
#define FACULTY_DB DATA_DIR "facultyDB.txt"
#define CLASS_COUNT_DB DATA_DIR "classCount.txt"
#define QUIZ_DB DATA_DIR "quiz.txt"
#define SGPA_DB DATA_DIR "sgpa.txt"

typedef struct {
    int id, semester, attendance;
    float marks, cgpa;
    char name[50], dept[20], grade[3];
} Student;

typedef struct {
    char question[200];
    char optA[100], optB[100], optC[100], optD[100];
    char correct;
} Quiz;

/* ---------- Basic helpers ---------- */

void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressEnter(void) {
    char input[8];
    printf("\nPress Enter to continue...");
    fgets(input, sizeof(input), stdin);
}

int fileExists(const char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
        return 0;

    fclose(fp);
    return 1;
}

void setupDataDirectory(void) {
    char path[MAX_PATH];

    if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0) {
        char *slash = strrchr(path, '\\');

        if (slash != NULL) {
            *slash = '\0';
            SetCurrentDirectoryA(path);
        }
    }

    _mkdir("data");
}

void initializeDataFiles(void) {
    FILE *fp;

    if (!fileExists(STUDENT_DB)) {
        fp = fopen(STUDENT_DB, "w");
        if (fp != NULL) {
            fprintf(fp, "1001 DemoStudent CSE 1 0 0.00 F 0.00\n");
            fclose(fp);
        }
    }

    if (!fileExists(FACULTY_DB)) {
        fp = fopen(FACULTY_DB, "w");
        if (fp != NULL) {
            fprintf(fp, "9001 Faculty CSE\n");
            fclose(fp);
        }
    }

    if (!fileExists(CLASS_COUNT_DB)) {
        fp = fopen(CLASS_COUNT_DB, "w");
        if (fp != NULL) {
            fprintf(fp, "0\n");
            fclose(fp);
        }
    }

    if (!fileExists(QUIZ_DB)) {
        fp = fopen(QUIZ_DB, "w");
        if (fp != NULL)
            fclose(fp);
    }

    if (!fileExists(SGPA_DB)) {
        fp = fopen(SGPA_DB, "w");
        if (fp != NULL)
            fclose(fp);
    }
}

int readInt(const char *prompt, int min, int max) {
    char input[64];
    int value;
    char extra;

    do {
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL)
            continue;

        if (sscanf(input, "%d %c", &value, &extra) == 1 &&
            value >= min && value <= max)
            return value;

        printf("  Invalid input. Enter a value from %d to %d.\n", min, max);
    } while (1);
}

float readFloat(const char *prompt, float min, float max) {
    char input[64];
    float value;
    char extra;

    do {
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL)
            continue;

        if (sscanf(input, "%f %c", &value, &extra) == 1 &&
            value >= min && value <= max)
            return value;

        printf("  Invalid input. Enter a value from %.2f to %.2f.\n", min, max);
    } while (1);
}

char readChoice(const char *prompt, const char *validChoices) {
    char input[16];
    char choice;

    do {
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL)
            continue;

        if (sscanf(input, " %c", &choice) == 1) {
            choice = (char)toupper((unsigned char)choice);

            if (strlen(input) >= 2 && strchr(validChoices, choice) != NULL)
                return choice;
        }

        printf("  Invalid choice. Valid options: %s\n", validChoices);
    } while (1);
}

void readText(const char *prompt, char *text, int size) {
    do {
        printf("%s", prompt);

        if (fgets(text, size, stdin) == NULL)
            continue;

        text[strcspn(text, "\r\n")] = '\0';

        if (text[0] == '\0') {
            printf("  This field cannot be empty.\n");
            continue;
        }

        if (strchr(text, '|') != NULL) {
            printf("  The character '|' is not allowed.\n");
            continue;
        }

        return;
    } while (1);
}

/* ---------- UI ---------- */

void headerf(const char *title) {
    system("cls");

    printf("\n==================================================================\n");
    printf("||                  ACADEMIC SYSTEM PORTAL                      ||\n");
    printf("==================================================================\n");

    if (title != NULL) {
        printf("||  %-60s||\n", title);
        printf("==================================================================\n");
    }
}

void successMessage(const char *message) {
    printf("\n[OK] %s\n", message);
}

void errorMessage(const char *message) {
    printf("\n[ERROR] %s\n", message);
}

/* ---------- Student data ---------- */

int loadStudents(Student s[]) {
    FILE *fp = fopen(STUDENT_DB, "r");
    int n = 0;

    if (fp == NULL)
        return 0;

    while (n < MAX_STUDENTS &&
           fscanf(fp, "%d %49s %19s %d %d %f %2s %f",
                  &s[n].id, s[n].name, s[n].dept,
                  &s[n].semester, &s[n].attendance,
                  &s[n].marks, s[n].grade, &s[n].cgpa) == 8) {
        n++;
    }

    fclose(fp);
    return n;
}

int saveStudents(Student s[], int n) {
    FILE *fp = fopen(STUDENT_DB, "w");

    if (fp == NULL)
        return 0;

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n",
                s[i].id, s[i].name, s[i].dept, s[i].semester,
                s[i].attendance, s[i].marks, s[i].grade, s[i].cgpa);
    }

    fclose(fp);
    return 1;
}

int getTotalClasses(void) {
    FILE *fp = fopen(CLASS_COUNT_DB, "r");
    int totalClass = 0;

    if (fp != NULL) {
        if (fscanf(fp, "%d", &totalClass) != 1)
            totalClass = 0;

        fclose(fp);
    }

    return totalClass >= 0 ? totalClass : 0;
}

int saveTotalClasses(int totalClass) {
    FILE *fp = fopen(CLASS_COUNT_DB, "w");

    if (fp == NULL)
        return 0;

    fprintf(fp, "%d\n", totalClass);
    fclose(fp);

    return 1;
}

void sortStudents(Student s[], int n, int byMarks) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int swap = byMarks
                       ? (s[j].marks < s[j + 1].marks)
                       : (s[j].id > s[j + 1].id);

            if (swap) {
                Student temp = s[j];
                s[j] = s[j + 1];
                s[j + 1] = temp;
            }
        }
    }
}

void printStudent(Student *s, int totalClass) {
    float attendancePercent = 0.0f;

    if (totalClass > 0)
        attendancePercent = (s->attendance * 100.0f) / totalClass;

    printf("\n+--------------------------------------------------------------+\n");
    printf("| ID         : %-47d |\n", s->id);
    printf("| Name       : %-47s |\n", s->name);
    printf("| Department : %-47s |\n", s->dept);
    printf("| Semester   : %-47d |\n", s->semester);
    printf("| Attendance : %d (%6.2f%%)%36s |\n",
           s->attendance, attendancePercent, "");
    printf("| Marks      : %-47.2f |\n", s->marks);
    printf("| Grade      : %-47s |\n", s->grade);
    printf("| CGPA       : %-47.2f |\n", s->cgpa);
    printf("+--------------------------------------------------------------+\n");
}

int findStudent(Student s[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (s[i].id == id)
            return i;
    }

    return -1;
}

void assignGrade(Student *s) {
    if (s->marks >= 80)
        strcpy(s->grade, "A+");
    else if (s->marks >= 75)
        strcpy(s->grade, "A");
    else if (s->marks >= 70)
        strcpy(s->grade, "A-");
    else if (s->marks >= 65)
        strcpy(s->grade, "B+");
    else if (s->marks >= 60)
        strcpy(s->grade, "B");
    else if (s->marks >= 55)
        strcpy(s->grade, "B-");
    else if (s->marks >= 50)
        strcpy(s->grade, "C+");
    else if (s->marks >= 45)
        strcpy(s->grade, "C");
    else if (s->marks >= 40)
        strcpy(s->grade, "D");
    else
        strcpy(s->grade, "F");
}

/* ---------- Main ---------- */

void teacherPortal(void);           void stdPortal(int userid);

void addStd(void);                  void stdrecords(int userid);
void searchStd(void);               void stopwatch(void);
void updateStd(void);               void utilities(int userid);
void deleteStd(void);               void sgpa(int userid);
void showStds(void);                void goalTracker(int userid);
void marksInput(void);              void quiz(int userid);
void addQuiz(void);
void attendance(void);


int facultyLogin(char faculty[], char dept[]) {
    FILE *fp = fopen(FACULTY_DB, "r");
    int userId, fileId;
    char fileFaculty[50], fileDept[20];

    if (fp == NULL)
        return 0;

    userId = readInt("Faculty ID: ", 1, 999999999);

    while (fscanf(fp, "%d %49s %19s",
                  &fileId, fileFaculty, fileDept) == 3) {
        if (fileId == userId) {
            strcpy(faculty, fileFaculty);
            strcpy(dept, fileDept);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int main(void) {
    int userIn;

    setupDataDirectory();
    initializeDataFiles();

    headerf("Welcome");
    printf("\n  [1] Faculty\n");
    printf("  [2] Student\n");
    printf("  [0] Exit\n\n");

    userIn = readInt("  Select your role: ", 0, 2);

    if (userIn == 1) {
        char faculty[50], dept[20];

        headerf("Faculty Login");

        if (facultyLogin(faculty, dept)) {
            printf("\n  Welcome, %s (%s Department)\n", faculty, dept);
            Sleep(1000);
            teacherPortal();
        } else {
            errorMessage("Access denied. Faculty ID not found.");
            pressEnter();
        }
    }
    else if (userIn == 2) {
        int userid;

        headerf("Student Login");
        userid = readInt("  Student ID: ", 1, 999999999);
        stdPortal(userid);
    }

    return 0;
}

/* ---------- Faculty portal ---------- */

void teacherPortal(void) {
    int choice;

    do {
        headerf("Faculty Portal");

        printf("\n  [1] Add Student\n");
        printf("  [2] Update Student Record\n");
        printf("  [3] Delete Student Record\n");
        printf("  [4] Attendance\n");
        printf("  [5] Find Student\n");
        printf("  [6] Show Student Records\n");
        printf("  [7] Input Exam Marks\n");
        printf("  [8] Quiz Manager\n");
        printf("  [0] Logout\n\n");

        choice = readInt("  Select an option: ", 0, 8);

        switch (choice) {
        case 1: addStd();      break;
        case 2: updateStd();   break;
        case 3: deleteStd();   break;
        case 4: attendance();  break;
        case 5: searchStd();   break;
        case 6: showStds();    break;
        case 7: marksInput();  break;
        case 8: addQuiz();     break;
        case 0: return;
        }
    } while (1);
}

void addStd(void) {
    Student s[MAX_STUDENTS];
    Student student;
    int n = loadStudents(s);
    int totalClass = getTotalClasses();

    headerf("Add Student");

    if (n >= MAX_STUDENTS) {
        errorMessage("Student limit reached.");
        pressEnter();
        return;
    }

    student.id = readInt("  Student ID: ", 1, 999999999);

    if (findStudent(s, n, student.id) != -1) {
        errorMessage("That student ID already exists.");
        pressEnter();
        return;
    }

    readText("  Name: ", student.name, sizeof(student.name));
    readText("  Department: ", student.dept, sizeof(student.dept));

    student.semester = readInt("  Semester (1-20): ", 1, 20);
    student.attendance = readInt("  Attendance count: ",
                                 0, totalClass > 0 ? totalClass : 10000);
    student.marks = readFloat("  Marks (0-100): ", 0, 100);
    student.cgpa = readFloat("  CGPA (0-4): ", 0, 4);

    assignGrade(&student);
    s[n] = student;

    if (saveStudents(s, n + 1))
        successMessage("Student added successfully.");
    else
        errorMessage("Could not save the student record.");

    pressEnter();
}

void searchStd(void) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int searchId, index;

    headerf("Find Student");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    searchId = readInt("  Student ID: ", 1, 999999999);
    index = findStudent(s, n, searchId);

    if (index == -1)
        errorMessage("Student not found.");
    else
        printStudent(&s[index], getTotalClasses());

    pressEnter();
}

void updateStd(void) {
    Student s[MAX_STUDENTS];
    char name[50], dept[20];
    int n = loadStudents(s);
    int searchId, i;
    int totalClass = getTotalClasses();

    headerf("Update Student");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    searchId = readInt("  Student ID: ", 1, 999999999);
    i = findStudent(s, n, searchId);

    if (i == -1) {
        errorMessage("Student not found.");
        pressEnter();
        return;
    }

    printf("\n  Current record:\n");
    printStudent(&s[i], totalClass);

    printf("\n  Enter new information:\n");
    readText("  Name: ", name, sizeof(name));
    readText("  Department: ", dept, sizeof(dept));

    strcpy(s[i].name, name);
    strcpy(s[i].dept, dept);
    s[i].semester = readInt("  Semester (1-20): ", 1, 20);
    s[i].attendance = readInt("  Attendance count: ",
                              0, totalClass > 0 ? totalClass : 10000);
    s[i].marks = readFloat("  Marks (0-100): ", 0, 100);
    s[i].cgpa = readFloat("  CGPA (0-4): ", 0, 4);

    assignGrade(&s[i]);

    if (saveStudents(s, n))
        successMessage("Student record updated successfully.");
    else
        errorMessage("Could not save the updated record.");

    pressEnter();
}

void deleteStd(void) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int searchId, index;

    headerf("Delete Student");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    searchId = readInt("  Student ID: ", 1, 999999999);
    index = findStudent(s, n, searchId);

    if (index == -1) {
        errorMessage("Student not found.");
        pressEnter();
        return;
    }

    printStudent(&s[index], getTotalClasses());

    if (readChoice("\n  Delete this record? (Y/N): ", "YN") != 'Y') {
        printf("\n  Deletion cancelled.\n");
        pressEnter();
        return;
    }

    for (int i = index; i < n - 1; i++)
        s[i] = s[i + 1];

    if (saveStudents(s, n - 1))
        successMessage("Student record deleted successfully.");
    else
        errorMessage("Could not save the student database.");

    pressEnter();
}

void showStds(void) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int choice;
    int totalClass;

    headerf("Student Records");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    printf("\n  [1] Sort by ID\n");
    printf("  [2] Sort by Marks\n\n");
    choice = readInt("  Sort by: ", 1, 2);

    for (int i = 0; i < n; i++)
        assignGrade(&s[i]);

    sortStudents(s, n, choice == 2);
    totalClass = getTotalClasses();

    printf("\n%-8s %-18s %-8s %-5s %-15s %-7s %-6s\n",
           "ID", "Name", "Dept", "Sem", "Attendance", "Grade", "CGPA");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        float percent = totalClass > 0
                        ? (s[i].attendance * 100.0f) / totalClass
                        : 0.0f;

        printf("%-8d %-18s %-8s %-5d %5d (%6.2f%%) %-7s %.2f\n",
               s[i].id, s[i].name, s[i].dept, s[i].semester,
               s[i].attendance, percent, s[i].grade, s[i].cgpa);
    }

    pressEnter();
}

void attendance(void) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);

    headerf("Attendance");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    int totalClass = getTotalClasses() + 1;

    printf("\n  Class #%d\n", totalClass);
    printf("  Mark P for Present or A for Absent.\n\n");

    for (int i = 0; i < n; i++) {
        char prompt[100];

        snprintf(prompt, sizeof(prompt),
                 "  %d - %s (P/A): ", s[i].id, s[i].name);

        if (readChoice(prompt, "PA") == 'P')
            s[i].attendance++;
    }

    if (!saveStudents(s, n)) {
        errorMessage("Student attendance could not be saved.");
        pressEnter();
        return;
    }

    if (!saveTotalClasses(totalClass))
        errorMessage("Attendance saved, but class count could not be updated.");
    else {
        printf("\n  Total Classes Taken: %d\n", totalClass);
        successMessage("Attendance updated successfully.");
    }

    pressEnter();
}

void marksInput(void) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    char choice;

    headerf("Exam Marks");

    if (n == 0) {
        errorMessage("No student records found.");
        pressEnter();
        return;
    }

    choice = readChoice("  Reset all marks first? (Y/N): ", "YN");

    if (choice == 'Y') {
        if (readChoice("  Confirm reset? (Y/N): ", "YN") == 'Y') {
            for (int i = 0; i < n; i++) {
                s[i].marks = 0;
                assignGrade(&s[i]);
            }

            if (saveStudents(s, n))
                successMessage("All marks have been reset.");
            else
                errorMessage("Could not save the reset marks.");

            pressEnter();
            return;
        }

        printf("\n  Reset cancelled.\n");
        pressEnter();
        return;
    }

    printf("\n  Enter additional marks for each student.\n");
    printf("  Use 0 when no marks should be added.\n\n");

    for (int i = 0; i < n; i++) {
        char prompt[120];
        float remaining = 100.0f - s[i].marks;

        snprintf(prompt, sizeof(prompt),
                 "  %d - %s additional marks (0-%.2f): ",
                 s[i].id, s[i].name, remaining);

        s[i].marks += readFloat(prompt, 0, remaining);
        assignGrade(&s[i]);
    }

    if (saveStudents(s, n))
        successMessage("Marks saved and grades updated.");
    else
        errorMessage("Could not save marks.");

    pressEnter();
}

/* ---------- Student portal ---------- */

void stdPortal(int userid) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int index = findStudent(s, n, userid);
    int choice;

    if (index == -1) {
        headerf("Student Login");
        errorMessage("Student ID not found.");
        pressEnter();
        return;
    }

    do {
        headerf("Student Portal");

        printf("\n  Welcome, %s (%s)\n\n", s[index].name, s[index].dept);
        printf("  [1] My Academic Record\n");
        printf("  [2] Utilities\n");
        printf("  [3] Quiz\n");
        printf("  [0] Logout\n\n");

        choice = readInt("  Select an option: ", 0, 3);

        switch (choice) {
        case 1: stdrecords(userid); break;
        case 2: utilities(userid); break;
        case 3: quiz(userid); break;
        case 0: return;
        }
    } while (1);
}

void stdrecords(int userid) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int index = findStudent(s, n, userid);

    headerf("My Academic Record");

    if (index == -1)
        errorMessage("Your record could not be found.");
    else
        printStudent(&s[index], getTotalClasses());

    pressEnter();
}

void utilities(int userid) {
    int choice;

    do {
        headerf("Student Utilities");

        printf("\n  [1] Study Timer\n");
        printf("  [2] SGPA Calculator\n");
        printf("  [3] Goal Tracker\n");
        printf("  [0] Return\n\n");

        choice = readInt("  Select an option: ", 0, 3);

        switch (choice) {
        case 1: stopwatch(); break;
        case 2: sgpa(userid); break;
        case 3: goalTracker(userid); break;
        case 0: return;
        }
    } while (1);
}

/* ---------- Quiz ---------- */

void addQuiz(void) {
    FILE *fp = fopen(QUIZ_DB, "a");
    Quiz q;
    int n;

    headerf("Quiz Manager");

    if (fp == NULL) {
        errorMessage("Could not open the quiz database.");
        pressEnter();
        return;
    }

    n = readInt("  Number of questions to add (1-50): ", 1, 50);

    for (int i = 0; i < n; i++) {
        printf("\n  Question %d\n", i + 1);

        readText("  Question: ", q.question, sizeof(q.question));
        readText("  Option A: ", q.optA, sizeof(q.optA));
        readText("  Option B: ", q.optB, sizeof(q.optB));
        readText("  Option C: ", q.optC, sizeof(q.optC));
        readText("  Option D: ", q.optD, sizeof(q.optD));

        q.correct = readChoice("  Correct option (A/B/C/D): ", "ABCD");

        fprintf(fp, "%s|%s|%s|%s|%s|%c\n",
                q.question, q.optA, q.optB,
                q.optC, q.optD, q.correct);
    }

    fclose(fp);

    successMessage("Quiz questions added successfully.");
    pressEnter();
}

void quiz(int userid) {
    FILE *fp = fopen(QUIZ_DB, "r");
    Quiz q;
    char buffer[MAX_LINE];
    int score = 0, total = 0;

    headerf("Quiz");

    if (fp == NULL) {
        errorMessage("No quiz is available.");
        pressEnter();
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        char answer;

        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (sscanf(buffer, "%199[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]| %c",
                   q.question, q.optA, q.optB,
                   q.optC, q.optD, &q.correct) != 6)
            continue;

        printf("\nQ%d. %s\n", total + 1, q.question);
        printf("  A. %s\n", q.optA);
        printf("  B. %s\n", q.optB);
        printf("  C. %s\n", q.optC);
        printf("  D. %s\n", q.optD);

        answer = readChoice("  Your answer: ", "ABCD");

        if (answer == (char)toupper((unsigned char)q.correct))
            score++;

        total++;
    }

    fclose(fp);

    if (total == 0)
        printf("\n  No valid quiz questions are available.\n");
    else
        printf("\n  Your Score: %d/%d (%.1f%%)\n",
               score, total, score * 100.0f / total);

    /* Quiz remains an independent activity; it does not alter marks. */
    (void)userid;
    pressEnter();
}

/* ---------- Utilities ---------- */

void stopwatch(void) {
    int minutes;

    headerf("Study Timer");

    minutes = readInt("  Study time (minutes, 1-180): ", 1, 180);

    printf("\n  Timer started for %d minute%s.\n",
           minutes, minutes == 1 ? "" : "s");

    for (int remaining = minutes * 60; remaining > 0; remaining--) {
        printf("\r  Time Left: %02d:%02d",
               remaining / 60, remaining % 60);

        fflush(stdout);
        Sleep(1000);
    }

    printf("\r  Time Left: 00:00\n");
    successMessage("Time's up! Take a break.");
    pressEnter();
}

void sgpa(int userid) {
    int n;
    float sum = 0.0f;
    float totalCredit = 0.0f;

    headerf("SGPA Calculator");

    n = readInt("  Number of courses (1-20): ", 1, 20);

    for (int i = 1; i <= n; i++) {
        float credit, grade;
        char prompt[80];

        snprintf(prompt, sizeof(prompt),
                 "  Course %d credit (0.1-10): ", i);
        credit = readFloat(prompt, 0.1f, 10.0f);

        snprintf(prompt, sizeof(prompt),
                 "  Course %d grade point (0-4): ", i);
        grade = readFloat(prompt, 0.0f, 4.0f);

        sum += credit * grade;
        totalCredit += credit;
    }

    if (totalCredit <= 0) {
        errorMessage("Invalid total credits.");
        pressEnter();
        return;
    }

    float newSGPA = sum / totalCredit;

    printf("\n  SGPA = %.2f\n", newSGPA);

    if (readChoice("  Save this SGPA to history? (Y/N): ", "YN") == 'Y') {
        FILE *fp = fopen(SGPA_DB, "a");

        if (fp != NULL) {
            fprintf(fp, "%d %.2f\n", userid, newSGPA);
            fclose(fp);
            successMessage("SGPA saved.");
        } else {
            errorMessage("Could not save SGPA history.");
        }
    }

    /* SGPA is only a calculation/history feature.
       It does not automatically change CGPA or semester. */

    pressEnter();
}

void goalTracker(int userid) {
    Student s[MAX_STUDENTS];
    int n = loadStudents(s);
    int index = findStudent(s, n, userid);
    float targetCGPA;

    headerf("CGPA Goal Tracker");

    if (index == -1) {
        errorMessage("Student not found.");
        pressEnter();
        return;
    }

    targetCGPA = readFloat("  Target CGPA (0-4): ", 0, 4);

    printf("\n  Current CGPA : %.2f\n", s[index].cgpa);
    printf("  Target CGPA  : %.2f\n", targetCGPA);

    if (s[index].cgpa >= targetCGPA)
        successMessage("Goal achieved! Keep it up.");
    else
        printf("\n  You need %.2f more to reach your goal.\n",
               targetCGPA - s[index].cgpa);

    pressEnter();
}
