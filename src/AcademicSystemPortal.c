/*
 * Academic System Portal
 * A small, file-based academic management system written in C.
 *
 * The implementation intentionally stays in one source file so it remains
 * approachable for C students while keeping clear section boundaries.
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define PATH_SEP "\\"
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATH_SEP "/"
#endif

#define MAX_STUDENTS 100
#define MAX_NAME 64
#define MAX_DEPT 32
#define MAX_LINE 512
#define MAX_PATH_LEN 1024

typedef struct {
    int id;
    char name[MAX_NAME];
    char dept[MAX_DEPT];
    int semester;
    int attendance;
    float marks;
    char grade[3];
    float cgpa;
} Student;

typedef struct {
    char question[200];
    char optA[100], optB[100], optC[100], optD[100];
    char correct;
} Quiz;

static char app_dir[MAX_PATH_LEN];
static char data_dir[MAX_PATH_LEN];
static char student_db[MAX_PATH_LEN];
static char faculty_db[MAX_PATH_LEN];
static char class_count_db[MAX_PATH_LEN];
static char quiz_db[MAX_PATH_LEN];
static char sgpa_db[MAX_PATH_LEN];

/* ---------- Platform helpers ---------- */
static void sleep_ms(unsigned int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    clock_t start = clock();
    clock_t ticks = (clock_t)((ms / 1000.0) * CLOCKS_PER_SEC);
    while (clock() - start < ticks) { }
#endif
}

static void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void make_directory(const char *path) {
#ifdef _WIN32
    _mkdir(path);
#else
    mkdir(path, 0755);
#endif
}

static void set_application_directory(void) {
#ifdef _WIN32
    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        if (_getcwd(app_dir, sizeof(app_dir)) == NULL) strcpy(app_dir, ".");
    } else {
        char *slash = strrchr(path, '\\');
        if (!slash) slash = strrchr(path, '/');
        if (slash) *slash = '\0';
        strncpy(app_dir, path, sizeof(app_dir) - 1);
        app_dir[sizeof(app_dir) - 1] = '\0';
    }
#else
    if (getcwd(app_dir, sizeof(app_dir)) == NULL) strcpy(app_dir, ".");
#endif

    snprintf(data_dir, sizeof(data_dir), "%s%sdata", app_dir, PATH_SEP);
    snprintf(student_db, sizeof(student_db), "%sstudents.txt", data_dir);
    snprintf(faculty_db, sizeof(faculty_db), "%sfaculty.txt", data_dir);
    snprintf(class_count_db, sizeof(class_count_db), "%sclass_count.txt", data_dir);
    snprintf(quiz_db, sizeof(quiz_db), "%squiz.txt", data_dir);
    snprintf(sgpa_db, sizeof(sgpa_db), "%ssgpa.txt", data_dir);
    make_directory(data_dir);
}

/* ---------- Input helpers ---------- */
static void discard_remaining_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void trim_newline(char *text) { text[strcspn(text, "\r\n")] = '\0'; }

static int read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (!fgets(buffer, (int)size, stdin)) return 0;
    if (!strchr(buffer, '\n')) discard_remaining_input();
    trim_newline(buffer);
    return 1;
}

static int parse_int(const char *text, int *value) {
    char *end;
    long parsed;
    errno = 0;
    parsed = strtol(text, &end, 10);
    while (isspace((unsigned char)*end)) end++;
    if (text == end || *end != '\0' || errno != 0) return 0;
    if (parsed < -2147483647L - 1 || parsed > 2147483647L) return 0;
    *value = (int)parsed;
    return 1;
}

static int parse_float(const char *text, float *value) {
    char *end;
    float parsed;
    errno = 0;
    parsed = strtof(text, &end);
    while (isspace((unsigned char)*end)) end++;
    if (text == end || *end != '\0' || errno != 0) return 0;
    *value = parsed;
    return 1;
}

static int read_int_range(const char *prompt, int min, int max) {
    char input[64];
    int value;
    for (;;) {
        if (!read_line(prompt, input, sizeof(input))) continue;
        if (parse_int(input, &value) && value >= min && value <= max) return value;
        printf("  Invalid input. Enter a number from %d to %d.\n", min, max);
    }
}

static float read_float_range(const char *prompt, float min, float max) {
    char input[64];
    float value;
    for (;;) {
        if (!read_line(prompt, input, sizeof(input))) continue;
        if (parse_float(input, &value) && value >= min && value <= max) return value;
        printf("  Invalid input. Enter a value from %.2f to %.2f.\n", min, max);
    }
}

static char read_choice(const char *prompt, const char *valid) {
    char input[32];
    for (;;) {
        if (!read_line(prompt, input, sizeof(input))) continue;
        if (strlen(input) == 1) {
            char choice = (char)toupper((unsigned char)input[0]);
            if (strchr(valid, choice)) return choice;
        }
        printf("  Invalid choice. Valid options: %s\n", valid);
    }
}

static void pause_screen(void) {
    char dummy[8];
    printf("\nPress Enter to continue...");
    fgets(dummy, sizeof(dummy), stdin);
}

/* ---------- UI helpers ---------- */
static void line(int width, char fill) {
    for (int i = 0; i < width; i++) putchar(fill);
    putchar('\n');
}

static void banner(const char *title, const char *subtitle) {
    clear_screen();
    printf("\n");
    line(64, '=');
    printf("                 ACADEMIC SYSTEM PORTAL\n");
    line(64, '=');
    if (title && *title) printf("  %s\n", title);
    if (subtitle && *subtitle) printf("  %s\n", subtitle);
    line(64, '-');
}

static void success(const char *message) { printf("\n[OK] %s\n", message); }
static void error_message(const char *message) { printf("\n[ERROR] %s\n", message); }

/* ---------- File helpers ---------- */
static int file_exists(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    fclose(fp);
    return 1;
}

static int write_default_file(const char *path, const char *content) {
    FILE *fp = fopen(path, "w");
    if (!fp) return 0;
    fputs(content, fp);
    fclose(fp);
    return 1;
}

static void initialize_data_files(void) {
    if (!file_exists(student_db)) write_default_file(student_db, "1001|Demo Student|CSE|1|0|0.00|F|0.00\n");
    if (!file_exists(faculty_db)) write_default_file(faculty_db, "9001|Demo Faculty|CSE\n");
    if (!file_exists(class_count_db)) write_default_file(class_count_db, "0\n");
    if (!file_exists(quiz_db)) write_default_file(quiz_db, "");
    if (!file_exists(sgpa_db)) write_default_file(sgpa_db, "");
}

static int replace_file(const char *temp_path, const char *target_path) {
#ifdef _WIN32
    return MoveFileExA(temp_path, target_path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(temp_path, target_path) == 0;
#endif
}

/* ---------- Student data ---------- */
static int load_students(Student students[]) {
    FILE *fp = fopen(student_db, "r");
    char buffer[MAX_LINE];
    int count = 0;
    if (!fp) return 0;

    while (count < MAX_STUDENTS && fgets(buffer, sizeof(buffer), fp)) {
        Student *s = &students[count];
        trim_newline(buffer);
        if (sscanf(buffer, "%d|%63[^|]|%31[^|]|%d|%d|%f|%2[^|]|%f",
                   &s->id, s->name, s->dept, &s->semester, &s->attendance,
                   &s->marks, s->grade, &s->cgpa) == 8) count++;
    }
    fclose(fp);
    return count;
}

static int save_students(const Student students[], int count) {
    char temp_path[MAX_PATH_LEN];
    FILE *fp;
    if (strlen(student_db) + 5 >= sizeof(temp_path)) return 0;
    strcpy(temp_path, student_db);
    strcat(temp_path, ".tmp");
    fp = fopen(temp_path, "w");
    if (!fp) return 0;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%d|%d|%.2f|%s|%.2f\n", students[i].id,
                students[i].name, students[i].dept, students[i].semester,
                students[i].attendance, students[i].marks, students[i].grade,
                students[i].cgpa);
    }
    fclose(fp);
    return replace_file(temp_path, student_db);
}

static int find_student(const Student students[], int count, int id) {
    for (int i = 0; i < count; i++) if (students[i].id == id) return i;
    return -1;
}

static void assign_grade(Student *student) {
    if (student->marks >= 80) strcpy(student->grade, "A+");
    else if (student->marks >= 75) strcpy(student->grade, "A");
    else if (student->marks >= 70) strcpy(student->grade, "A-");
    else if (student->marks >= 65) strcpy(student->grade, "B+");
    else if (student->marks >= 60) strcpy(student->grade, "B");
    else if (student->marks >= 55) strcpy(student->grade, "B-");
    else if (student->marks >= 50) strcpy(student->grade, "C+");
    else if (student->marks >= 45) strcpy(student->grade, "C");
    else if (student->marks >= 40) strcpy(student->grade, "D");
    else strcpy(student->grade, "F");
}

static int get_total_classes(void) {
    FILE *fp = fopen(class_count_db, "r");
    int total = 0;
    if (fp) {
        if (fscanf(fp, "%d", &total) != 1) total = 0;
        fclose(fp);
    }
    return total >= 0 ? total : 0;
}

static int save_total_classes(int total) {
    FILE *fp = fopen(class_count_db, "w");
    if (!fp) return 0;
    fprintf(fp, "%d\n", total);
    fclose(fp);
    return 1;
}

static void print_student(const Student *student) {
    int total = get_total_classes();
    float attendance = total > 0 ? student->attendance * 100.0f / total : 0.0f;
    printf("\n+--------------------------------------------------------------+\n");
    printf("| Student ID : %-45d |\n", student->id);
    printf("| Name       : %-45s |\n", student->name);
    printf("| Department : %-45s |\n", student->dept);
    printf("| Semester   : %-45d |\n", student->semester);
    printf("| Attendance : %-5d (%6.2f%%)%31s |\n", student->attendance, attendance, "");
    printf("| Marks      : %-45.2f |\n", student->marks);
    printf("| Grade      : %-45s |\n", student->grade);
    printf("| CGPA       : %-45.2f |\n", student->cgpa);
    printf("+--------------------------------------------------------------+\n");
}

/* ---------- Faculty ---------- */
static int faculty_login(char *name, char *dept, size_t name_size, size_t dept_size) {
    FILE *fp = fopen(faculty_db, "r");
    char buffer[MAX_LINE];
    int wanted = read_int_range("Faculty ID: ", 1, 999999999);
    int id;
    if (!fp) return 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        trim_newline(buffer);
        if (sscanf(buffer, "%d|%63[^|]|%31[^
]", &id, name, dept) == 3 && id == wanted) {
            name[name_size - 1] = '\0';
            dept[dept_size - 1] = '\0';
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

static void add_student(void) {
    Student students[MAX_STUDENTS], student;
    char buffer[MAX_NAME];
    int count = load_students(students);
    banner("Add Student", "Create a new student record.");

    if (count >= MAX_STUDENTS) { error_message("Student limit reached."); pause_screen(); return; }
    student.id = read_int_range("Student ID: ", 1, 999999999);
    if (find_student(students, count, student.id) >= 0) {
        error_message("A student with that ID already exists."); pause_screen(); return;
    }

    do { read_line("Name: ", buffer, sizeof(buffer)); if (strchr(buffer, '|') || !*buffer) printf("  Name cannot be empty or contain '|'.\n"); }
    while (strchr(buffer, '|') || !*buffer);
    strcpy(student.name, buffer);
    do { read_line("Department: ", buffer, sizeof(buffer)); if (strchr(buffer, '|') || !*buffer) printf("  Department cannot be empty or contain '|'.\n"); }
    while (strchr(buffer, '|') || !*buffer);
    strcpy(student.dept, buffer);

    student.semester = read_int_range("Semester (1-20): ", 1, 20);
    student.attendance = read_int_range("Attendance count: ", 0, 10000);
    student.marks = read_float_range("Marks (0-100): ", 0, 100);
    student.cgpa = read_float_range("CGPA (0-4): ", 0, 4);
    assign_grade(&student);
    students[count++] = student;

    if (save_students(students, count)) success("Student added successfully.");
    else error_message("Could not save student data.");
    pause_screen();
}

static void search_student(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students);
    int id, index;
    banner("Find Student", "Search by student ID.");
    id = read_int_range("Student ID: ", 1, 999999999);
    index = find_student(students, count, id);
    if (index < 0) error_message("Student record not found.");
    else print_student(&students[index]);
    pause_screen();
}

static void update_student(void) {
    Student students[MAX_STUDENTS];
    char buffer[MAX_NAME];
    int count = load_students(students), id, index;
    banner("Update Student", "Edit an existing student record.");
    id = read_int_range("Student ID: ", 1, 999999999);
    index = find_student(students, count, id);
    if (index < 0) { error_message("Student record not found."); pause_screen(); return; }

    printf("\nEditing: %s\n\n", students[index].name);
    do { read_line("Name: ", buffer, sizeof(buffer)); if (strchr(buffer, '|') || !*buffer) printf("  Name cannot be empty or contain '|'.\n"); }
    while (strchr(buffer, '|') || !*buffer);
    strcpy(students[index].name, buffer);
    do { read_line("Department: ", buffer, sizeof(buffer)); if (strchr(buffer, '|') || !*buffer) printf("  Department cannot be empty or contain '|'.\n"); }
    while (strchr(buffer, '|') || !*buffer);
    strcpy(students[index].dept, buffer);
    students[index].semester = read_int_range("Semester (1-20): ", 1, 20);
    students[index].attendance = read_int_range("Attendance count: ", 0, 10000);
    students[index].marks = read_float_range("Marks (0-100): ", 0, 100);
    students[index].cgpa = read_float_range("CGPA (0-4): ", 0, 4);
    assign_grade(&students[index]);

    if (save_students(students, count)) success("Student updated successfully.");
    else error_message("Could not save student data.");
    pause_screen();
}

static void delete_student(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students), id, index;
    banner("Delete Student", "Remove a student record.");
    id = read_int_range("Student ID: ", 1, 999999999);
    index = find_student(students, count, id);
    if (index < 0) { error_message("Student record not found."); pause_screen(); return; }
    print_student(&students[index]);
    if (read_choice("\nDelete this record? (Y/N): ", "YN") != 'Y') { printf("Deletion cancelled.\n"); pause_screen(); return; }
    for (int i = index; i < count - 1; i++) students[i] = students[i + 1];
    if (save_students(students, count - 1)) success("Student deleted successfully.");
    else error_message("Could not save student data.");
    pause_screen();
}

static void show_students(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students);
    banner("Student Records", "View and sort all student records.");
    if (count == 0) { error_message("No student records found."); pause_screen(); return; }
    int sort_choice = read_int_range("Sort by [1] ID  [2] Marks: ", 1, 2);
    for (int i = 0; i < count; i++) assign_grade(&students[i]);
    for (int i = 0; i < count - 1; i++) for (int j = i + 1; j < count; j++) {
        int swap = sort_choice == 1 ? students[i].id > students[j].id : students[i].marks < students[j].marks;
        if (swap) { Student temp = students[i]; students[i] = students[j]; students[j] = temp; }
    }
    printf("\n%-8s %-22s %-10s %-5s %-14s %-7s %-6s\n", "ID", "Name", "Dept", "Sem", "Attendance", "Grade", "CGPA");
    line(78, '-');
    int total = get_total_classes();
    for (int i = 0; i < count; i++) {
        float attendance = total > 0 ? students[i].attendance * 100.0f / total : 0.0f;
        printf("%-8d %-22.22s %-10.10s %-5d %5d (%6.2f%%) %-7s %.2f\n", students[i].id, students[i].name, students[i].dept, students[i].semester, students[i].attendance, attendance, students[i].grade, students[i].cgpa);
    }
    pause_screen();
}

static void take_attendance(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students);
    int total_classes = get_total_classes() + 1;
    banner("Attendance", "Mark attendance for the next class.");
    if (count == 0) { error_message("No student records found."); pause_screen(); return; }
    printf("Class #%d\nEnter P for Present or A for Absent.\n\n", total_classes);
    for (int i = 0; i < count; i++) {
        char prompt[128];
        snprintf(prompt, sizeof(prompt), "%d - %s (P/A): ", students[i].id, students[i].name);
        if (read_choice(prompt, "PA") == 'P') students[i].attendance++;
    }
    if (!save_students(students, count) || !save_total_classes(total_classes)) error_message("Attendance could not be saved completely.");
    else { printf("\nTotal classes recorded: %d\n", total_classes); success("Attendance saved successfully."); }
    pause_screen();
}

static void input_marks(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students);
    banner("Exam Marks", "Enter marks and update grades.");
    if (count == 0) { error_message("No student records found."); pause_screen(); return; }
    if (read_choice("Reset all current marks to zero? (Y/N): ", "YN") == 'Y') {
        if (read_choice("Confirm reset? This cannot be undone. (Y/N): ", "YN") == 'Y') {
            for (int i = 0; i < count; i++) { students[i].marks = 0; assign_grade(&students[i]); }
            printf("\nAll marks reset.\n");
        } else printf("\nReset cancelled.\n");
    }
    printf("\nEnter additional marks for each student.\nUse 0 when no marks should be added.\n\n");
    for (int i = 0; i < count; i++) {
        char prompt[128];
        snprintf(prompt, sizeof(prompt), "%d - %s additional marks (0-%.2f): ", students[i].id, students[i].name, 100.0f - students[i].marks);
        students[i].marks += read_float_range(prompt, 0, 100.0f - students[i].marks);
        assign_grade(&students[i]);
    }
    if (save_students(students, count)) success("Marks saved and grades updated.");
    else error_message("Could not save marks.");
    pause_screen();
}

static void add_quiz(void) {
    FILE *fp = fopen(quiz_db, "a");
    int count;
    Quiz q;
    banner("Quiz Manager", "Add questions to the student quiz.");
    if (!fp) { error_message("Could not open quiz database."); pause_screen(); return; }
    count = read_int_range("Number of questions: ", 1, 100);
    for (int i = 0; i < count; i++) {
        printf("\nQuestion %d\n", i + 1);
        do { read_line("Question: ", q.question, sizeof(q.question)); } while (!*q.question || strchr(q.question, '|'));
        do { read_line("Option A: ", q.optA, sizeof(q.optA)); } while (!*q.optA || strchr(q.optA, '|'));
        do { read_line("Option B: ", q.optB, sizeof(q.optB)); } while (!*q.optB || strchr(q.optB, '|'));
        do { read_line("Option C: ", q.optC, sizeof(q.optC)); } while (!*q.optC || strchr(q.optC, '|'));
        do { read_line("Option D: ", q.optD, sizeof(q.optD)); } while (!*q.optD || strchr(q.optD, '|'));
        q.correct = read_choice("Correct option (A/B/C/D): ", "ABCD");
        fprintf(fp, "%s|%s|%s|%s|%s|%c\n", q.question, q.optA, q.optB, q.optC, q.optD, q.correct);
    }
    fclose(fp);
    success("Quiz questions added successfully.");
    pause_screen();
}

/* ---------- Student utilities ---------- */
static void show_my_records(int id) {
    Student students[MAX_STUDENTS];
    int count = load_students(students), index;
    banner("My Academic Record", "Your current academic information.");
    index = find_student(students, count, id);
    if (index < 0) error_message("Your student record could not be found."); else print_student(&students[index]);
    pause_screen();
}

static void sgpa_calculator(int id) {
    int courses = read_int_range("Number of courses (1-20): ", 1, 20);
    float weighted_sum = 0, total_credits = 0;
    banner("SGPA Calculator", "Calculate and optionally save this semester's SGPA.");
    for (int i = 1; i <= courses; i++) {
        char prompt[64];
        float credit, grade_point;
        snprintf(prompt, sizeof(prompt), "Course %d credit: ", i); credit = read_float_range(prompt, .1f, 10);
        snprintf(prompt, sizeof(prompt), "Course %d grade point: ", i); grade_point = read_float_range(prompt, 0, 4);
        weighted_sum += credit * grade_point; total_credits += credit;
    }
    { float sgpa = weighted_sum / total_credits;
      printf("\nYour SGPA: %.2f\n", sgpa);
      if (read_choice("Save this SGPA to history? (Y/N): ", "YN") == 'Y') {
          FILE *fp = fopen(sgpa_db, "a");
          if (fp) { fprintf(fp, "%d %.2f\n", id, sgpa); fclose(fp); success("SGPA saved."); }
          else error_message("Could not save SGPA history.");
      }
    }
    pause_screen();
}

static void goal_tracker(int id) {
    Student students[MAX_STUDENTS];
    int count = load_students(students), index;
    float target;
    banner("CGPA Goal Tracker", "Compare your current CGPA with your target.");
    index = find_student(students, count, id);
    if (index < 0) { error_message("Your student record could not be found."); pause_screen(); return; }
    target = read_float_range("Target CGPA (0-4): ", 0, 4);
    printf("\nCurrent CGPA : %.2f\nTarget CGPA  : %.2f\n", students[index].cgpa, target);
    if (students[index].cgpa >= target) success("Goal achieved! Keep it up.");
    else printf("\nRemaining gap: %.2f\nKeep working toward your target.\n", target - students[index].cgpa);
    pause_screen();
}

static void study_timer(void) {
    int minutes;
    banner("Study Timer", "Simple countdown timer.");
    minutes = read_int_range("Study time in minutes (1-180): ", 1, 180);
    printf("\nTimer started for %d minute%s.\n", minutes, minutes == 1 ? "" : "s");
    for (int remaining = minutes * 60; remaining > 0; remaining--) {
        printf("\rTime remaining: %02d:%02d", remaining / 60, remaining % 60); fflush(stdout); sleep_ms(1000);
    }
    printf("\rTime remaining: 00:00\n"); success("Time's up! Take a break."); pause_screen();
}

static void run_quiz(void) {
    FILE *fp = fopen(quiz_db, "r");
    Quiz q;
    char buffer[MAX_LINE];
    int score = 0, total = 0;
    banner("Quiz", "Answer the available questions.");
    if (!fp) { error_message("Quiz data could not be opened."); pause_screen(); return; }
    while (fgets(buffer, sizeof(buffer), fp)) {
        trim_newline(buffer);
        if (sscanf(buffer, "%199[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]| %c", q.question, q.optA, q.optB, q.optC, q.optD, &q.correct) != 6) continue;
        printf("\nQ%d. %s\n  A. %s\n  B. %s\n  C. %s\n  D. %s\n", ++total, q.question, q.optA, q.optB, q.optC, q.optD);
        if (read_choice("Answer: ", "ABCD") == (char)toupper((unsigned char)q.correct)) score++;
    }
    fclose(fp);
    if (total == 0) printf("\nNo quiz questions are available.\n");
    else printf("\nFinal score: %d/%d (%.1f%%)\n", score, total, score * 100.0f / total);
    pause_screen();
}

/* ---------- Menus ---------- */
static void faculty_portal(const char *faculty_name, const char *dept) {
    for (;;) {
        banner("Faculty Portal", "Manage student records and academic data.");
        printf("  Logged in as: %s (%s)\n\n", faculty_name, dept);
        printf("  [1] Add Student\n  [2] Update Student\n  [3] Delete Student\n  [4] Attendance\n  [5] Find Student\n  [6] Show Students\n  [7] Input Marks\n  [8] Quiz Manager\n  [0] Logout\n\n");
        switch (read_int_range("  Select an option: ", 0, 8)) {
            case 1: add_student(); break; case 2: update_student(); break; case 3: delete_student(); break;
            case 4: take_attendance(); break; case 5: search_student(); break; case 6: show_students(); break;
            case 7: input_marks(); break; case 8: add_quiz(); break; case 0: return;
        }
    }
}

static void student_portal(int id) {
    for (;;) {
        Student students[MAX_STUDENTS];
        int count = load_students(students), index = find_student(students, count, id);
        if (index < 0) { banner("Student Portal", NULL); error_message("Your student record could not be found."); pause_screen(); return; }
        banner("Student Portal", "Academic tools and personal information.");
        printf("  Welcome, %s\n\n  [1] My Academic Record\n  [2] SGPA Calculator\n  [3] CGPA Goal Tracker\n  [4] Study Timer\n  [5] Quiz\n  [0] Logout\n\n", students[index].name);
        switch (read_int_range("  Select an option: ", 0, 5)) {
            case 1: show_my_records(id); break; case 2: sgpa_calculator(id); break; case 3: goal_tracker(id); break;
            case 4: study_timer(); break; case 5: run_quiz(); break; case 0: return;
        }
    }
}

int main(void) {
    char faculty_name[MAX_NAME], dept[MAX_DEPT];
    set_application_directory();
    initialize_data_files();
    banner("Welcome", "A simple academic management system in C.");
    printf("\n  [1] Faculty\n  [2] Student\n  [0] Exit\n\n");
    switch (read_int_range("  Select your role: ", 0, 2)) {
        case 1:
            banner("Faculty Login", "Enter your faculty ID.");
            if (faculty_login(faculty_name, dept, sizeof(faculty_name), sizeof(dept))) {
                printf("\nWelcome, %s.\n", faculty_name); sleep_ms(500); faculty_portal(faculty_name, dept);
            } else { error_message("Access denied. Faculty ID not found."); pause_screen(); }
            break;
        case 2: student_portal(read_int_range("  Student ID: ", 1, 999999999)); break;
        case 0: break;
    }
    return 0;
}
