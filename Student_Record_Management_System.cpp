#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>

using namespace std;

const int SUBJECT_COUNT = 5;
const int MAX_MARKS = 100;
const string FILE_NAME = "students.txt";

class Student {
private:
    int rollNumber;
    string name;
    string course;
    int marks[SUBJECT_COUNT];

public:
    Student() {
        rollNumber = 0;
        name = "";
        course = "";

        for (int i = 0; i < SUBJECT_COUNT; i++) {
            marks[i] = 0;
        }
    }

    Student(int roll, const string& studentName,
            const string& studentCourse,
            const int studentMarks[]) {

        rollNumber = roll;
        name = studentName;
        course = studentCourse;

        for (int i = 0; i < SUBJECT_COUNT; i++) {
            marks[i] = studentMarks[i];
        }
    }

    int getRollNumber() const {
        return rollNumber;
    }

    string getName() const {
        return name;
    }

    string getCourse() const {
        return course;
    }

    int getTotal() const {
        int total = 0;

        for (int i = 0; i < SUBJECT_COUNT; i++) {
            total += marks[i];
        }

        return total;
    }

    double getPercentage() const {
        return (getTotal() * 100.0) /
               (SUBJECT_COUNT * MAX_MARKS);
    }

    string getGrade() const {
        double percentage = getPercentage();

        if (percentage >= 90) return "A+";
        if (percentage >= 80) return "A";
        if (percentage >= 70) return "B";
        if (percentage >= 60) return "C";
        if (percentage >= 50) return "D";

        return "F";
    }

    void updateDetails(const string& newName,
                       const string& newCourse) {
        name = newName;
        course = newCourse;
    }

    void updateMarks(const int newMarks[]) {
        for (int i = 0; i < SUBJECT_COUNT; i++) {
            marks[i] = newMarks[i];
        }
    }

    void display() const {
        cout << "\n----------------------------------------\n";
        cout << "Roll Number : " << rollNumber << '\n';
        cout << "Name        : " << name << '\n';
        cout << "Course      : " << course << '\n';

        cout << "Subject Marks : ";

        for (int i = 0; i < SUBJECT_COUNT; i++) {
            cout << marks[i];

            if (i != SUBJECT_COUNT - 1) {
                cout << ", ";
            }
        }

        cout << "\nTotal       : " << getTotal() << " / 500";
        cout << "\nPercentage  : "
             << fixed << setprecision(2)
             << getPercentage() << "%";
        cout << "\nGrade       : " << getGrade();
        cout << "\n----------------------------------------\n";
    }

    void saveToFile(ofstream& outFile) const {
        outFile << rollNumber << ' '
                << quoted(name) << ' '
                << quoted(course);

        for (int i = 0; i < SUBJECT_COUNT; i++) {
            outFile << ' ' << marks[i];
        }

        outFile << '\n';
    }

    bool loadFromFile(ifstream& inFile) {

        if (!(inFile >> rollNumber
                     >> quoted(name)
                     >> quoted(course))) {
            return false;
        }

        for (int i = 0; i < SUBJECT_COUNT; i++) {

            if (!(inFile >> marks[i])) {
                return false;
            }

            if (marks[i] < 0 || marks[i] > MAX_MARKS) {
                return false;
            }
        }

        return true;
    }
};

class StudentManager {
private:
    vector<Student> students;

    int findStudentIndex(int roll) const {

        for (int i = 0; i < static_cast<int>(students.size()); i++) {

            if (students[i].getRollNumber() == roll) {
                return i;
            }
        }

        return -1;
    }

    int readInteger(const string& prompt,
                    int minimum,
                    int maximum) const {

        int value;

        while (true) {

            cout << prompt;

            if (cin >> value &&
                value >= minimum &&
                value <= maximum) {

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n');

                return value;
            }

            cout << "Invalid input. Enter a number from "
                 << minimum << " to "
                 << maximum << ".\n";

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n');
        }
    }

    string readText(const string& prompt) const {

        string value;

        while (true) {

            cout << prompt;
            getline(cin, value);

            if (!value.empty()) {
                return value;
            }

            cout << "This field cannot be empty.\n";
        }
    }

    void readSubjectMarks(int newMarks[]) const {

        for (int i = 0; i < SUBJECT_COUNT; i++) {

            ostringstream prompt;

            prompt << "Enter marks for subject "
                   << i + 1
                   << " (0-100): ";

            newMarks[i] = readInteger(
                prompt.str(),
                0,
                MAX_MARKS);
        }
    }

public:
    StudentManager() {
        loadRecords();
    }

    void loadRecords() {

        ifstream inFile(FILE_NAME.c_str());

        if (!inFile) {
            return;
        }

        while (true) {

            Student record;

            if (!record.loadFromFile(inFile)) {
                break;
            }

            if (findStudentIndex(
                    record.getRollNumber()) == -1) {

                students.push_back(record);
            }
        }

        inFile.close();
    }

    void saveRecords() const {

        ofstream outFile(FILE_NAME.c_str());

        if (!outFile) {
            cout << "Error saving records.\n";
            return;
        }

        for (size_t i = 0; i < students.size(); i++) {
            students[i].saveToFile(outFile);
        }

        outFile.close();
    }

    void addStudent() {

        cout << "\n--- Add Student ---\n";

        int roll =
            readInteger("Enter roll number: ",
                        1,
                        1000000000);

        if (findStudentIndex(roll) != -1) {
            cout << "Student already exists.\n";
            return;
        }

        string name =
            readText("Enter student name: ");

        string course =
            readText("Enter course: ");

        int marks[SUBJECT_COUNT];

        readSubjectMarks(marks);

        students.push_back(
            Student(roll, name, course, marks));

        saveRecords();

        cout << "Student added successfully.\n";
    }

    void displayAllStudents() const {

        cout << "\n--- All Student Records ---\n";

        if (students.empty()) {
            cout << "No records available.\n";
            return;
        }

        cout << "Total Students: "
             << students.size() << "\n";

        for (size_t i = 0; i < students.size(); i++) {
            students[i].display();
        }
    }

    void searchStudent() const {

        cout << "\n--- Search Student ---\n";

        int roll =
            readInteger(
                "Enter roll number to search: ",
                1,
                1000000000);

        int index =
            findStudentIndex(roll);

        if (index == -1) {
            cout << "Student not found.\n";
        } else {
            students[index].display();
        }
    }

    void updateStudent() {

        cout << "\n--- Update Student ---\n";

        int roll =
            readInteger(
                "Enter roll number to update: ",
                1,
                1000000000);

        int index =
            findStudentIndex(roll);

        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }

        cout << "\n1. Update Name & Course\n";
        cout << "2. Update Marks\n";
        cout << "3. Update Both\n";

        int choice =
            readInteger("Enter choice: ",
                        1,
                        3);

        if (choice == 1 || choice == 3) {

            string newName =
                readText("Enter new name: ");

            string newCourse =
                readText("Enter new course: ");

            students[index].updateDetails(
                newName,
                newCourse);
        }

        if (choice == 2 || choice == 3) {

            int marks[SUBJECT_COUNT];

            readSubjectMarks(marks);

            students[index].updateMarks(marks);
        }

        saveRecords();

        cout << "Record updated successfully.\n";
    }

    void deleteStudent() {

        cout << "\n--- Delete Student ---\n";

        int roll =
            readInteger(
                "Enter roll number to delete: ",
                1,
                1000000000);

        int index =
            findStudentIndex(roll);

        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }

        char confirm;

        cout << "Are you sure (Y/N)? ";
        cin >> confirm;
        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n');

        if (toupper(confirm) == 'Y') {

            students.erase(
                students.begin() + index);

            saveRecords();

            cout << "Record deleted successfully.\n";
        }
    }

    void run() {

        int choice;

        do {

            cout << "\n========== STUDENT RECORD MANAGEMENT SYSTEM ==========\n";
            cout << "1. Add Student\n";
            cout << "2. Display All Students\n";
            cout << "3. Search Student\n";
            cout << "4. Update Student\n";
            cout << "5. Delete Student\n";
            cout << "6. Exit\n";
            cout << "=====================================================\n";

            choice =
                readInteger(
                    "Enter your choice (1-6): ",
                    1,
                    6);

            switch (choice) {

                case 1:
                    addStudent();
                    break;

                case 2:
                    displayAllStudents();
                    break;

                case 3:
                    searchStudent();
                    break;

                case 4:
                    updateStudent();
                    break;

                case 5:
                    deleteStudent();
                    break;

                case 6:
                    saveRecords();
                    cout << "Records saved.\n";
                    break;
            }

        } while (choice != 6);
    }
};

int main() {

    StudentManager manager;
    manager.run();

    return 0;
}