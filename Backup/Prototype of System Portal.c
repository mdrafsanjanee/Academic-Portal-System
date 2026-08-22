#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>

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

void teacherPortal(); void stdPortal(int userid); void attendance(); void stdrecords(int userid); void marksInput();
void addStd(); void searchStd(); void updateStd(); void deleteStd(); void showStds(); void assignGrade(Student *s);
void utilities(int userid);void stopwatch(int userid); void quiz(int userid); void addQuiz();void sgpa(int userid);
void CGPA(int userid, float newSGPA); void goalTracker(int userid);

int main(){

    printf("============================\n\n");
    printf("|| Academic System Portal ||\n\n");
    printf("============================");

    Sleep(1200);
    system("cls");

    int userIn;
    do {
    printf("What are you?\n1. Faculty\n2. Student\nEnter your role: ");
    scanf("%d", &userIn);
    if (userIn != 1 && userIn != 2) {
        system("cls");
        printf("Invalid Input\n");
        Sleep(500);
    }
    } while (userIn != 1 && userIn != 2);

    switch (userIn) {
    case 1:
    system("cls");

    int tuserid, file_id = 0;
    char faculty[50], tdept[20];

    printf("Welcome to the Faculty Portal\n");
    printf("Enter your ID: ");
    scanf("%d", &tuserid);

    FILE *fc = fopen("facultyDB.txt", "r");

    if (fc == NULL) {
        printf("File not found\n");
        return 0;
    }

    int found = 0;

    while (fscanf(fc, "%d %s %s", &file_id, faculty, tdept) != EOF) {
        if (file_id == tuserid) {
            found = 1;
            break;
        }
    }

    fclose(fc);

    if (found) {
        system("cls");
        printf("Welcome %s (%s Department)\n", faculty, tdept);
        Sleep(1000);
        teacherPortal();
    } else {
        printf("Access denied\n");
        Sleep(1000);
        system("cls");
        main();
    }
    break;
    
    case 2:
        system("cls");
        int userid;
        printf("Welcome to the Student Portal.\nWhat is your ID?:\n");
        scanf("%d", &userid);
        stdPortal(userid);
        break;
    default:
        break;
    }
    return 0;
}

void teacherPortal() {
    system("cls");
    int teacherIn;
    do {
    printf("What would you like to do?\n1. Add student\n2. Update Student Record\n3. Delete Student Record\n4. Attendence\n5. Find by ID\n6. Show all Students\n7. Input Exam Marks\n8. Add Quiz Questions\n9. Exit\nEnter your choice: ");
    scanf("%d", &teacherIn);
    if (teacherIn < 1 || teacherIn > 9){
        printf("\nInvalid Choice!\n");
        Sleep(600);
        system("cls");
    }
    } while (teacherIn < 1 || teacherIn > 9);
    switch(teacherIn) {
    case 1:
        addStd();
        break;
    case 2:
        updateStd(); 
        break;
    case 3:
        deleteStd();
        break;
    case 4:
        attendance();
        break;
    case 5:
        searchStd();
        break;
    case 6:
        showStds();
        break;
    case 7:
        marksInput();
        break;
    case 8:
        addQuiz();
        break;
    case 9:
        return;
    default:
        break;
    }
    
}

void stdPortal(int userid) {
    system("cls");
    
    FILE *fp = fopen("stdDB.txt", "r");
    if (fp == NULL) {
    printf("File not found\n");
    return;
    }
    Student s;
    int found = 0;
    getchar();

    while (fscanf(fp, "%d %s %s %d %d %f %s %f", &s.id, s.name, s.dept, &s.semester, &s.attendance, &s.marks, s.grade, &s.cgpa) != EOF) {

        if (s.id == userid) {
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (found == 0) {
        printf("Student not found\n");
        Sleep(1000);
        system("cls");
        main();
    } else {
    printf("Welcome %s from %s\n", s.name, s.dept);
    
    int stdIn;
    do {
    printf("Main Menu:\n1. Your Records\n2. Utilities\n3. Quiz\n4. Exit\nYour Choice: ");
    scanf("%d", &stdIn);
    } while (stdIn < 1 || stdIn > 4);

    switch(stdIn){
        case 1:
            stdrecords(userid);
            break;
        case 2:
            utilities(userid);
            break;
        case 3:
            quiz(userid);
            break;
        case 4:
            return;
        default:
            break;
    }
    }
}

void addStd() {

    FILE *fp = fopen("stdDB.txt", "a");

    if (fp == NULL) {
        printf("Failed to open file\n");
        return;
    }

    Student s1;

    printf("Enter: ID Name Dept Semester Attendance Marks Grade CGPA:\n");
    scanf("%d %49s %19s %d %d %f %3s %f", &s1.id, s1.name, s1.dept, &s1.semester, &s1.attendance, &s1.marks, s1.grade, &s1.cgpa);

    fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n", s1.id, s1.name, s1.dept, s1.semester, s1.attendance, s1.marks, s1.grade, s1.cgpa);

    printf("Student added Successfully\n");

    fclose(fp);

    fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
        printf("No records found\n");
        return;
    }

    Student s[100];
    int n = 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %2s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        n++;
    }
    fclose(fp);
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (s[j].id > s[j+1].id) {
                Student temp = s[j];
                s[j] = s[j+1];
                s[j+1] = temp;
            }
        }
    }
    for (int i = 0; i < n; i++) {
            assignGrade(&s[i]);
        }

    fp = fopen("stdDB.txt", "w");
    if (fp == NULL) {
        printf("Failed Failed");
    }
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n", s[i].id, s[i].name, s[i].dept, s[i].semester, s[i].attendance, s[i].marks, s[i].grade, s[i].cgpa);
    }
    fclose(fp);

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void searchStd() {

    FILE *fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
    printf("File not found\n");
    return;
    }
    FILE *fc = fopen("classCount.txt", "r");
    int totalClass = 1;
    if (fc != NULL) {
        fscanf(fc, "%d", &totalClass);
        fclose(fc);
    }

    Student s[100];
    int search_id;

    printf("Enter ID to search: ");
    scanf("%d", &search_id);

    int found = 0, n= 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        if (s[n].id == search_id) {
            system("cls");
            printf("Student Found:\nID: %03d\nName: %s\nDept: %s\nSemester: %d\nAttendance: %d (%0.2f %%)\nMarks: %.2f\nGrade: %s\nCGPA: %.2f\n", s[n].id, s[n].name, s[n].dept, s[n].semester, s[n].attendance, ((s[n].attendance*100.0)/totalClass), s[n].marks, s[n].grade, s[n].cgpa);
            found++;
            break;
        }
        n++;
    }

    if (found == 0) {
        printf("Student not found\n");
    }

    fclose(fp);
    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void updateStd() {

    FILE *fp = fopen("stdDB.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL) {
        printf("File error\n");
        return;
    }

    Student s[100];
    int search_id;

    printf("Enter ID to update: ");
    scanf("%d", &search_id);

    int found = 0, n = 0;
    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        if (s[n].id == search_id) {
            printf("Enter NEW data:\nID Name Dept Semester Attendance Marks Grade CGPA:\n");
            scanf("%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa);
            assignGrade(&s[n]);
            found = 1;
        }
        fprintf(temp, "%d %s %s %d %d %.2f %s %.2f\n", s[n].id, s[n].name, s[n].dept, s[n].semester, s[n].attendance, s[n].marks, s[n].grade, s[n].cgpa);
        n++;
    }

    fclose(fp); fclose(temp);

    remove("stdDB.txt");
    rename("temp.txt", "stdDB.txt");

    if (found) {
        printf("Student updated successfully\n");
    } else {
        printf("Student was not found\n");
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void deleteStd() {
    
    FILE *fp = fopen("stdDB.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL) {
        printf("File error\n");
        return;
    }

    Student s[100];
    int search_id;

    printf("Enter ID to delete: ");
    scanf("%d", &search_id);

    int found = 0, n = 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        if (s[n].id == search_id) {
            found = 1;
        } else {
        fprintf(temp, "%d %s %s %d %d %.2f %s %.2f\n", s[n].id, s[n].name, s[n].dept, s[n].semester, s[n].attendance, s[n].marks, s[n].grade, s[n].cgpa);
        }
        n++;
    }

    fclose(fp); fclose(temp);

    remove("stdDB.txt");
    rename("temp.txt", "stdDB.txt");

    if (found) {
        printf("Student record deleted successfully\n");
    } else {
        printf("Student was not found\n");
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void showStds() {

    int sort = 1;
    do {
    printf("Sort by?\n1. Id\n2. Mark\nEnter choice: ");
    scanf("%d", &sort);
    } while (sort != 1 && sort != 2);

    FILE *fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
        printf("No records found\n");
        return;
    }

    Student s[100];
    int n = 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        n++;
    }
    for (int i = 0; i < n; i++) {
        assignGrade(&s[i]);
        }

    fclose(fp);

    if (sort != 1) {
        for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (s[j].marks < s[j+1].marks) {
                Student temp = s[j];
                s[j] = s[j+1];
                s[j+1] = temp;
            }
        }
    }
    }

    FILE *fc = fopen("classCount.txt", "r");

    int totalClass = 1;
    if (fc != NULL) {
        fscanf(fc, "%d", &totalClass);
        fclose(fc);
    }
    system("cls");
    printf("All student records:\n");
    for (int i = 0; i < n; i++) {
        printf("ID: %03d | Name: %-10s | Dept: %-3s | Semester: %-1d | Attendance: %-3d (%8.2f %%) | Marks: %-7.2f | Grade: %-4s | CGPA: %-3.2f\n", s[i].id, s[i].name, s[i].dept, s[i].semester, s[i].attendance, ((s[i].attendance *100.00) /totalClass) ,s[i].marks, s[i].grade, s[i].cgpa);
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void attendance() {

    int totalClass = 0;

    FILE *fc = fopen("classCount.txt", "r");

    if (fc != NULL) {
        fscanf(fc, "%d", &totalClass);
        fclose(fc);
    }
    totalClass++;

    FILE *fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
        printf("No records found\n");
        return;
    }

    Student s[100];
    int n = 0;
    char status;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        n++;
    }
    fclose(fp);
    printf("Taking attendance: \n");
    for (int i = 0; i < n; i++) {

        printf("%d %s (P/A): ", s[i].id, s[i].name);
        scanf(" %c", &status);

        if (status == 'P' || status == 'p') {
            s[i].attendance++;
        }
    }
    fp = fopen("stdDB.txt", "w");

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n", s[i].id, s[i].name, s[i].dept, s[i].semester, s[i].attendance, s[i].marks, s[i].grade, s[i].cgpa);
    }

    fclose(fp);

    fc = fopen("classCount.txt", "w");
    fprintf(fc, "%d", totalClass);
    fclose(fc);

    printf("Attendance updated successfully\n");
    printf("\nTotal Classes Taken: %d\n", totalClass);

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void marksInput() {

    FILE *fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
        printf("No records found\n");
        return;
    }

    Student s[100];
    int n = 0;
    float examMark = 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        n++;
    }
    fclose(fp);

    char reset = 'n', trash;
    printf("Do you want to reset marks? (Y/N)\n");
    scanf(" %c", &trash);
    if (trash == 'Y' || trash == 'y'){
    printf("WARNING: This will reset ALL marks.\nConfirm? (Y/N): ");
    scanf(" %c", &reset);
    }

    if(reset == 'Y' || reset == 'y') {
        for (int i = 0; i < n; i++) {
            s[i].marks = 0;
            assignGrade(&s[i]);
        }
    } else {
        printf("Input Marks:\n");
        for (int i = 0; i < n; i++) {

            printf("%d %s Marks: %0.2f + ", s[i].id, s[i].name, s[i].marks);
            scanf("%f", &examMark);
            s[i].marks += examMark;
            assignGrade(&s[i]);
        }
    }
    fp = fopen("stdDB.txt", "w");
    if (fp == NULL) {
        printf("Error writing file\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n", s[i].id, s[i].name, s[i].dept, s[i].semester, s[i].attendance, s[i].marks, s[i].grade, s[i].cgpa);
    }
    fclose(fp);

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void assignGrade(Student *s) {

    if (s->marks >= 80) {
        strcpy(s->grade, "A+");
    }
    else if (s->marks >= 75) {
        strcpy(s->grade, "A");
    }
    else if (s->marks >= 70) {
        strcpy(s->grade, "A-");
    }
    else if (s->marks >= 65) {
        strcpy(s->grade, "B+");
    }
    else if (s->marks >= 60) {
        strcpy(s->grade, "B");
    }
    else if (s->marks >= 55) {
        strcpy(s->grade, "B-");
    }
    else if (s->marks >= 50) {
        strcpy(s->grade, "C+");
    }
    else if (s->marks >= 45) {
        strcpy(s->grade, "C");
    }
    else if (s->marks >= 40) {
        strcpy(s->grade, "D");
    }
    else {
        strcpy(s->grade, "F");
    }
}

void addQuiz() {
    FILE *fp = fopen("quiz.txt", "w");
    if (fp == NULL) {
        printf("File error\n");
        return;
    }

    Quiz q;
    int n;
    printf("How many questions to add?: ");
    scanf("%d", &n);

    getchar(); // doesn't work without this. char buffer ig

    for (int i = 0; i < n; i++) {
        printf("\nQuestion %d:\n", i + 1);

        printf("Enter question: ");
        fgets(q.question, sizeof(q.question), stdin);
        q.question[strcspn(q.question, "\n")] = 0;

        printf("Option A: ");
        fgets(q.optA, sizeof(q.optA), stdin);
        q.optA[strcspn(q.optA, "\n")] = 0;

        printf("Option B: ");
        fgets(q.optB, sizeof(q.optB), stdin);
        q.optB[strcspn(q.optB, "\n")] = 0;

        printf("Option C: ");
        fgets(q.optC, sizeof(q.optC), stdin);
        q.optC[strcspn(q.optC, "\n")] = 0;

        printf("Option D: ");
        fgets(q.optD, sizeof(q.optD), stdin);
        q.optD[strcspn(q.optD, "\n")] = 0;

        do {
        printf("Correct option (A/B/C/D): ");
        scanf(" %c", &q.correct);
        q.correct = toupper(q.correct);
        } while (q.correct < 'A' || q.correct > 'D');
        getchar();

        fprintf(fp, "%s|%s|%s|%s|%s|%c\n",
            q.question, q.optA, q.optB, q.optC, q.optD, q.correct);
    }

    fclose(fp);
    printf("Questions added!\n");

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        teacherPortal();
    }
}

void stdrecords(int userid) {
    system("cls");
    FILE *fp = fopen("stdDB.txt", "r");
    if (fp == NULL) {
    printf("File not found\n");
    return;
    }
    FILE *fc = fopen("classCount.txt", "r");
    int totalClass = 1;
    if (fc != NULL) {
        fscanf(fc, "%d", &totalClass);
        fclose(fc);
    }
    Student s;
    int found = 0;

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s.id, s.name, s.dept, &s.semester, &s.attendance, &s.marks, s.grade, &s.cgpa) != EOF) {

        if (s.id == userid) {
            printf("Your Records:\nID: %03d\nName: %s\nDept: %s\nSemester: %d \nGrade: %s\nMarks: %0.2f\nAttendance: %d (%0.2f %%)\nCGPA: %.2f\n",s.id, s.name, s.dept, s.semester ,s.grade, s.marks, s.attendance, ((s.attendance *100.00) /totalClass), s.cgpa);
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    if (found == 0){
        printf("Records not found!\n");
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        stdPortal(userid);
    }
}

void utilities(int userid) {
    system("cls");
    int utilityIn;
    do {
    printf("What would you like to do?\n1. Study Timer\n2. SGPA Calculator\n3. Goal Tracker\n4. Return to Main Menu\nEnter your choice: ");
    scanf("%d", &utilityIn);
    } while (utilityIn < 1 || utilityIn > 4);

    switch (utilityIn) {
    case 1:
        stopwatch(userid);
        break;
    case 2:
        sgpa(userid);
        break;
    case 3:
        goalTracker(userid);
        break;
    case 4:
        stdPortal(userid);
        break;
    }
}

void quiz(int userid) {
    FILE *fp = fopen("quiz.txt", "r");
    if (fp == NULL) {
        printf("No quiz available\n");
        char junk;
        printf("Return to Main Menu? (Y/N)\n");
        scanf(" %c", &junk);
        if (junk == 'Y' || junk == 'y') {
        stdPortal(userid);
    }
        return;
    }

    Quiz q;
    int score = 0, total = 0;
    char ans;

    while (fscanf(fp, " %[^|]|%[^|]|%[^|]|%[^|]|%[^|]| %c", q.question, q.optA, q.optB, q.optC, q.optD, &q.correct) != EOF) {

        printf("\n%s\n", q.question);
        printf("A. %s\n", q.optA);
        printf("B. %s\n", q.optB);
        printf("C. %s\n", q.optC);
        printf("D. %s\n", q.optD);

        printf("Your answer: ");
        scanf(" %c", &ans);

        if (toupper(ans) == toupper(q.correct)) {
            score++;
        }
        total++;
    }

    fclose(fp);
    printf("\nYour Score: %d/%d\n", score, total);

    Student s[100];
    int n = 0;

    fp = fopen("stdDB.txt", "r");

    if (fp == NULL) {
        printf("No records found\n");
        return;
    }

    while (fscanf(fp, "%d %49s %19s %d %d %f %3s %f", &s[n].id, s[n].name, s[n].dept, &s[n].semester, &s[n].attendance, &s[n].marks, s[n].grade, &s[n].cgpa) != EOF) {
        if (n >= 100) {break;}
        n++;
    }
    fclose(fp);

    fp = fopen("stdDB.txt", "w");
    if (fp == NULL) {
        printf("Quiz Result not saved\nError writing file\n");
        return;
    }

    for (int i = 0; i < n; i++) {
    if (s[i].id == userid) {
        s[i].marks += score;
        assignGrade(&s[i]);
    }

    fprintf(fp, "%d %s %s %d %d %.2f %s %.2f\n", s[i].id, s[i].name, s[i].dept, s[i].semester, s[i].attendance, s[i].marks, s[i].grade, s[i].cgpa);
    }
    fclose(fp);

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        stdPortal(userid);
    }
}

void stopwatch(int userid) {
    system("cls");
    
    int minutes = 0, seconds = 0;
    printf("Enter study time (minutes): ");
    scanf("%d", &minutes);

    printf("Study Timer Started for %d minutes\n", minutes);

    while (minutes >= 0) {
        printf("\rTime Left: %02d:%02d", minutes, seconds);
        Sleep(1000);

        if (seconds == 0) {
            minutes--;
            seconds = 59;
        } else {
            seconds--;
        }
    }

    printf("\n\nTime's up! Take a break.\n");

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        utilities(userid);
    }

}

void sgpa(int userid) {
    int n;
    float sgpa, sum = 0, Tcredit = 0, credit, grade;
    do {
    printf("Number of Courses: ");
    scanf("%d", &n);
    } while (n <= 0);
    int i = 1;
    while (n > 0) {
        printf("Credit of Course #%d: ", i);
        scanf("%f", &credit);
        printf("Grade of Course #%d: ", i);
        scanf("%f", &grade);

        sum += (credit * grade);
        Tcredit += credit;
        n--; i++;
    }
    if (Tcredit == 0) {
        printf("Invalid total credits\n");
        return;
    }
    sgpa = (sum) / (Tcredit);
    printf("SGPA = %0.2f\n", sgpa);
    CGPA(userid, sgpa);

    FILE *fs = fopen("sgpa.txt", "a");

    if (fs != NULL) {
        fprintf(fs, "%d %.2f\n", userid, sgpa);
        fclose(fs);
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        utilities(userid);
    }
}

void CGPA(int userid, float newSGPA) {
    FILE *fp = fopen("stdDB.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL) {
        printf("File error\n");
        return;
    }

    Student s;

    while (fscanf(fp, "%d %49s %19s %d %d %f %2s %f", &s.id, s.name, s.dept, &s.semester, &s.attendance, &s.marks, s.grade, &s.cgpa) != EOF) {

        if (s.id == userid) { 
            s.cgpa = ((s.cgpa * (s.semester - 1)) + newSGPA) / (s.semester);
            s.semester++;
        }

        fprintf(temp, "%d %s %s %d %d %.2f %s %.2f\n", s.id, s.name, s.dept, s.semester, s.attendance, s.marks, s.grade, s.cgpa);
    }

    fclose(fp); fclose(temp);

    remove("stdDB.txt");
    rename("temp.txt", "stdDB.txt");
    }

void goalTracker(int userid) {
    system("cls");

    float targetCGPA, currentCGPA;

    printf("Enter your target CGPA: ");
    scanf("%f", &targetCGPA);

    FILE *fp = fopen("stdDB.txt", "r");
    Student s;

    while (fscanf(fp, "%d %s %s %d %d %f %2s %f", &s.id, s.name, s.dept, &s.semester, &s.attendance, &s.marks, s.grade, &s.cgpa) != EOF) {

        if (s.id == userid) {
            currentCGPA = s.cgpa;
            break;
        }
    }
    fclose(fp);

    printf("\nCurrent CGPA: %.2f\n", currentCGPA);
    printf("Target CGPA: %.2f\n", targetCGPA);

    if (currentCGPA >= targetCGPA) {
        printf("Congratulation!\nGoal Achieved!\n");
    } else {
        printf("You need %.2f more to reach your goal.\n", targetCGPA - currentCGPA);
    }

    char junk;
    printf("Return to Main Menu? (Y/N)\n");
    scanf(" %c", &junk);
    if (junk == 'Y' || junk == 'y') {
        utilities(userid);
    }
}