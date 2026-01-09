#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

/* =========================================================
   InputHelper
   - Centralized input validation
   - Prevents infinite loops on invalid input
   ========================================================= */
class InputHelper
{
public:
    // Clears error flags and removes invalid input
    static void clear()
    {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    // Reads input safely and validates type
    template <typename T>
    static bool read(T &value)
    {
        cin >> value;
        if (cin.fail())
        {
            clear();
            return false;
        }
        return true;
    }
};

/* =========================================================
   Address
   - Simple data structure (Value Object)
   ========================================================= */
struct Address
{
    string city;
    string street;
};

/* =========================================================
   Person (Abstract Base Class)
   - Common interface for all people
   - Enforces polymorphic behavior
   ========================================================= */
class Person
{
protected:
    string name;
    int id;
    Address address;

public:
    virtual ~Person() = default;

    // Display info to console
    virtual void display() const = 0;

    // Save record to file
    virtual void save(ofstream &out) const = 0;
};

/* =========================================================
   Student
   - Domain model for student entity
   ========================================================= */
class Student : public Person
{
private:
    float gpa;
    static int count; // Tracks active student objects

public:
    Student(const string &n, int i, const Address &addr, float g)
    {
        if (g < 0.0f || g > 4.0f)
            throw runtime_error("GPA must be between 0.0 and 4.0");

        name = n;
        id = i;
        address = addr;
        gpa = g;
        count++;
    }

    ~Student() { count--; }

    void display() const override
    {
        cout << "[Student] " << id
             << " | " << name
             << " | GPA: " << gpa << endl;
    }

    void save(ofstream &out) const override
    {
        out << "Student | " << id
            << " | " << name
            << " | GPA: " << gpa
            << " | City: " << address.city << endl;
    }

    static int getCount() { return count; }
};

int Student::count = 0;

/* =========================================================
   Professor
   - Domain model for professor entity
   ========================================================= */
class Professor : public Person
{
private:
    double salary;

public:
    Professor(const string &n, int i, const Address &addr, double s)
    {
        name = n;
        id = i;
        address = addr;
        salary = s;
    }

    void display() const override
    {
        cout << "[Professor] " << id
             << " | " << name
             << " | Salary: $" << salary << endl;
    }

    void save(ofstream &out) const override
    {
        out << "Professor | " << id
            << " | " << name
            << " | Salary: $" << salary
            << " | City: " << address.city << endl;
    }
};

/* =========================================================
   UniversityRepository (Template)
   - Manages storage and lifetime of objects
   - SRP: data management only
   ========================================================= */
template <typename T>
class UniversityRepository
{
private:
    vector<T *> records;

public:
    void add(T *obj)
    {
        records.push_back(obj);
    }

    void displayAll() const
    {
        if (records.empty())
        {
            cout << "No records found.\n";
            return;
        }
        for (auto r : records)
            r->display();
    }

    // Provides read-only access for saving
    const vector<T *> &getAll() const
    {
        return records;
    }

    ~UniversityRepository()
    {
        for (auto r : records)
            delete r;
    }
};

/* =========================================================
   FileService (Template)
   - Handles persistence (file saving)
   - Keeps file logic separate from business logic
   ========================================================= */
template <typename T>
class FileService
{
public:
    static void save(const string &filename, const vector<T *> &data)
    {
        ofstream out(filename);
        if (!out)
        {
            cout << "File error.\n";
            return;
        }

        for (auto obj : data)
            obj->save(out);

        out.close();
        cout << "Saved to " << filename << endl;
    }
};

/* =========================================================
   Menu
   - Handles user interaction only
   - No business or file logic inside
   ========================================================= */
class Menu
{
private:
    UniversityRepository<Student> students;
    UniversityRepository<Professor> professors;

public:
    // Entry point for the application
    void run()
    {
        int choice;
        do
        {
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Students\n2. Professors\n3. Exit\nChoice: ";

            if (!InputHelper::read(choice))
                continue;

            if (choice == 1)
                studentMenu();
            else if (choice == 2)
                professorMenu();

        } while (choice != 3);
    }

private:
    void studentMenu()
    {
        int choice;
        do
        {
            cout << "\n--- Student Menu ---\n";
            cout << "1. Add\n2. View\n3. Save\n4. Back\nChoice: ";

            if (!InputHelper::read(choice))
                continue;

            try
            {
                if (choice == 1)
                    addStudent();
                else if (choice == 2)
                    students.displayAll();
                else if (choice == 3)
                    FileService<Student>::save("students.txt", students.getAll());
            }
            catch (const exception &e)
            {
                cout << e.what() << endl;
            }

        } while (choice != 4);
    }

    void professorMenu()
    {
        int choice;
        do
        {
            cout << "\n--- Professor Menu ---\n";
            cout << "1. Add\n2. View\n3. Save\n4. Back\nChoice: ";

            if (!InputHelper::read(choice))
                continue;

            if (choice == 1)
                addProfessor();
            else if (choice == 2)
                professors.displayAll();
            else if (choice == 3)
                FileService<Professor>::save("professors.txt", professors.getAll());

        } while (choice != 4);
    }

    // Collects and validates student input
    void addStudent()
    {
        string name;
        Address addr;
        int id;
        float gpa;

        cout << "Name: ";
        cin >> name;
        cout << "ID: ";
        if (!InputHelper::read(id))
            return;
        cout << "City: ";
        cin >> addr.city;
        cout << "Street: ";
        cin >> addr.street;
        cout << "GPA: ";
        if (!InputHelper::read(gpa))
            return;

        students.add(new Student(name, id, addr, gpa));
    }

    // Collects and validates professor input
    void addProfessor()
    {
        string name;
        Address addr;
        int id;
        double salary;

        cout << "Name: ";
        cin >> name;
        cout << "ID: ";
        if (!InputHelper::read(id))
            return;
        cout << "City: ";
        cin >> addr.city;
        cout << "Street: ";
        cin >> addr.street;
        cout << "Salary: ";
        if (!InputHelper::read(salary))
            return;

        professors.add(new Professor(name, id, addr, salary));
    }
};

/* =========================================================
   Main
   ========================================================= */
int main()
{
    Menu app;
    app.run();
    return 0;
}
