#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

/* =========================================================
    InputHelper
    - Centralized input checker
   ========================================================= */
class InputHelper
{
public:
    // Resets error flags and flushes the input buffer (removes bad characters).
    static void clear()
    {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    // Safely attempts input. Returns false (and clears buffer) if the wrong type is entered.
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
    - Simple data structure
   ========================================================= */
struct Address
{
    string city;
    string street;
};

/* =========================================================
    Person (Abstract Base Class)
    - Common interface for all people
   ========================================================= */
class Person
{
protected:
    string name;
    int id;
    Address address;

public:
    Person() : id(0), name("Unknown") {}

    virtual ~Person() = default;

    virtual void display() const = 0;
    virtual void save(ofstream &out) const = 0;

    void setName(string n) { name = n; }
};

/* =========================================================
    Student
    - Domain model for student entity
   ========================================================= */
class Student : public Person
{
private:
    float gpa;
    float lastTestScores[3]; // Fixed size array for recent scores
    static int count;

public:
    Student()
    {
        gpa = 0.0;
        for (int i = 0; i < 3; i++)
            lastTestScores[i] = 0.0;
        count++;
    }

    Student(const string &n, int i, const Address &addr, float g)
    {
        if (g < 0.0f || g > 4.0f)
            throw runtime_error("GPA must be between 0.0 and 4.0");

        name = n;
        id = i;
        address = addr;
        gpa = g;

        // Initialize with default scores
        for (int k = 0; k < 3; k++)
            lastTestScores[k] = 10.0;
        count++;
    }

    ~Student() { count--; }

    void display() const override
    {
        cout << "[Student] " << id
             << " | " << name
             << " | GPA: " << gpa
             << " | City: " << address.city << endl;
    }

    // this function prints a line of text directly into the .txt file
    void save(ofstream &out) const override
    {
        out << "Student | " << id
            << " | " << name
            << " | GPA: " << gpa
            << " | City: " << address.city << endl;
    }

    // Overloaded method to update name
    void updateInfo(const string &newName)
    {
        this->name = newName;
        cout << "Name updated.\n";
    }

    // Overloaded method to update address
    void updateInfo(const Address &newAddr)
    {
        this->address = newAddr;
        cout << "Address updated.\n";
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
    Professor()
    {
        salary = 0.0;
    }

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

    // Returns a direct, read-only reference to the list to avoid changing details by mistake.
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
    - Handles persistence
   ========================================================= */
template <typename T>
class FileService
{
public:
    static void save(const string &filename, const vector<T *> &data)
    {
        ofstream out(filename);

        if (!out) // checks if the file opened correctly
        {
            cout << "File error.\n";
            return;
        }

        for (auto obj : data) // "auto" data type is used to encourage polymorphisim
            obj->save(out);

        out.close();
        cout << "Saved to " << filename << endl;
    }
};

/* =========================================================
    Menu
    - Handles user interaction
   ========================================================= */
class Menu
{
private:
    UniversityRepository<Student> students;
    UniversityRepository<Professor> professors;

public:
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