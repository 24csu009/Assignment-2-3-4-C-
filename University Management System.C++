
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdexcept>

using namespace std;



class UniversitySystemException : public exception {
    string msg;
public:
    UniversitySystemException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class EnrollmentException : public UniversitySystemException {
public:
    EnrollmentException(const string& m) : UniversitySystemException("Enrollment Error: " + m) {}
};

class GradeException : public UniversitySystemException {
public:
    GradeException(const string& m) : UniversitySystemException("Grade Error: " + m) {}
};

class PaymentException : public UniversitySystemException {
public:
    PaymentException(const string& m) : UniversitySystemException("Payment Error: " + m) {}
};

// -------------------- Core Classes --------------------

class Person {
protected:
    string name, contactInfo;
    int age, ID;

public:
    Person(string n, int a, int id, string contact) {
        if (n.empty()) throw UniversitySystemException("Name cannot be empty.");
        if (a <= 0 || a > 120) throw UniversitySystemException("Invalid age.");
        name = n; age = a; ID = id; contactInfo = contact;
    }

    virtual void displayDetails() {
        cout << "Name: " << name << ", Age: " << age << ", ID: " << ID << ", Contact: " << contactInfo << endl;
    }

    virtual double calculatePayment() { return 0.0; }
    virtual ~Person() {}
};

class Student : public Person {
protected:
    string enrollmentDate, program;
    double GPA;

public:
    Student(string n, int a, int id, string contact, string enroll, string prog, double gpa)
        : Person(n, a, id, contact), enrollmentDate(enroll), program(prog) {
        setGPA(gpa);
    }

    void setGPA(double g) {
        if (g < 0.0 || g > 4.0) throw UniversitySystemException("GPA must be between 0.0 and 4.0");
        GPA = g;
    }

    double getGPA() const { return GPA; }

    void displayDetails() override {
        Person::displayDetails();
        cout << "Program: " << program << ", GPA: " << GPA << ", Enrolled: " << enrollmentDate << endl;
    }

    double calculatePayment() override { return 10000.0; }  // Base fee in INR
};

class GraduateStudent : public Student {
    string researchTopic, advisor, thesisTitle;

public:
    GraduateStudent(string n, int a, int id, string contact, string enroll, string prog, double gpa,
                    string topic, string adv, string thesis)
        : Student(n, a, id, contact, enroll, prog, gpa),
          researchTopic(topic), advisor(adv), thesisTitle(thesis) {}

    void displayDetails() override {
        Student::displayDetails();
        cout << "Research Topic: " << researchTopic << ", Advisor: " << advisor << ", Thesis: " << thesisTitle << endl;
    }

    double calculatePayment() override { return 12000.0; }  // Graduate student fee in INR
};

class Professor : public Person {
protected:
    string department, specialization, hireDate;

public:
    Professor(string n, int a, int id, string contact, string dept, string spec, string hDate)
        : Person(n, a, id, contact), department(dept), specialization(spec), hireDate(hDate) {}

    void displayDetails() override {
        Person::displayDetails();
        cout << "Department: " << department << ", Specialization: " << specialization << ", Hire Date: " << hireDate << endl;
    }

    double calculatePayment() override { return 50000.0; }  // Basic professor salary in INR
};

class FullProfessor : public Professor {
    int yearsOfService;
    double researchGrants;

public:
    FullProfessor(string n, int a, int id, string contact, string dept, string spec, string hDate, int years, double grants)
        : Professor(n, a, id, contact, dept, spec, hDate), yearsOfService(years), researchGrants(grants) {}

    double calculatePayment() override {
        return 80000.0 + 1500 * yearsOfService + researchGrants;
    }
};

// -------------------- Course, Department, University --------------------

class Course {
    string code, title, description;
    int credits;
    Professor* instructor;

public:
    Course(string c, string t, int cr, string desc, Professor* inst)
        : code(c), title(t), credits(cr), description(desc), instructor(inst) {
        if (cr <= 0) throw UniversitySystemException("Course credits must be positive.");
    }

    string getCode() const { return code; }
};

class Department {
    string name, location;
    double budget;
    vector<Professor*> professors;

public:
    Department(string n, string loc, double b) : name(n), location(loc), budget(b) {}

    void addProfessor(Professor* p) {
        professors.push_back(p);
    }

    vector<Professor*> getProfessors() {
        return professors;
    }
};

class University {
    vector<Department*> departments;
    map<int, Student*> students;
    map<string, Course*> courses;

public:
    void addDepartment(Department* d) {
        departments.push_back(d);
    }

    void addStudent(Student* s) {
        students[s->calculatePayment()] = s;
    }

    void addCourse(Course* c) {
        courses[c->getCode()] = c;
    }

    void generateReport() {
        cout << "\n--- The NorthCap University Report ---" << endl;
        for (auto& entry : students) {
            Student* s = entry.second;
            s->displayDetails();
            cout << "Fees Due: ₹" << s->calculatePayment() << "\n---\n";
        }
    }

    ~University() {
        for (auto& entry : students) delete entry.second;
        for (auto& entry : courses) delete entry.second;
        for (auto* dept : departments) delete dept;
    }
};

// -------------------- GradeBook and Enrollment --------------------

class GradeBook {
    map<int, double> grades;

public:
    void addGrade(int studentID, double grade) {
        if (grade < 0 || grade > 100) throw GradeException("Invalid grade.");
        grades[studentID] = grade;
    }

    double calculateAverageGrade() {
        if (grades.empty()) return 0.0;
        double total = 0;
        for (auto& entry : grades)
            total += entry.second;
        return total / grades.size();
    }
};

class EnrollmentManager {
    map<string, vector<int>> courseEnrollments;

public:
    void enrollStudent(string courseCode, int studentID) {
        if (courseEnrollments[courseCode].size() >= 50)
            throw EnrollmentException("Course is full.");
        courseEnrollments[courseCode].push_back(studentID);
    }

    int getEnrollmentCount(string courseCode) {
        return courseEnrollments[courseCode].size();
    }
};

// -------------------- Main Test Program --------------------

int main() {
    try {
        University uni;

        // Create Professors
        FullProfessor* prof1 = new FullProfessor("Dr. Mehta", 55, 101, "mehta@northcap.edu", "CS", "AI", "2001", 20, 20000);
        Department* csDept = new Department("Computer Science", "Block B1", 500000);
        csDept->addProfessor(prof1);
        uni.addDepartment(csDept);

        // Create Course
        Course* course1 = new Course("CS101", "Intro to CS", 3, "Basics of computing", prof1);
        uni.addCourse(course1);

        // Add Students
        Student* s1 = new Student("Rohit", 20, 201, "rohit@northcap.edu", "2023-08", "CS", 3.5);
        GraduateStudent* s2 = new GraduateStudent("Sneha", 24, 202, "sneha@northcap.edu", "2022-08", "CS", 3.8, "AI in Healthcare", "Dr. Mehta", "Ethical Challenges in AI");
        uni.addStudent(s1);
        uni.addStudent(s2);

        // Enroll Students
        EnrollmentManager em;
        em.enrollStudent("CS101", 201);
        em.enrollStudent("CS101", 202);

        // Add Grades
        GradeBook gb;
        gb.addGrade(201, 85);
        gb.addGrade(202, 92);

        // Report
        uni.generateReport();
        cout << "Average Grade: " << gb.calculateAverageGrade() << "%" << endl;

    } catch (UniversitySystemException& ex) {
        cerr << "System Error: " << ex.what() << endl;
    } catch (exception& ex) {
        cerr << "General Error: " << ex.what() << endl;
    }

    return 0;
}
