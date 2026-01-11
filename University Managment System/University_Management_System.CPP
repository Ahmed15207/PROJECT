#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

class InputHelper
{
    /* =========================================================
    InputHelper Class

    Purpose: Provides centralized input validation utilities

    Design Pattern: Utility class with static methods

    Key Features:
    - Template-based type-safe input validation
    - Automatic error recovery and buffer clearing
    - Prevents infinite loops from type mismatches

    Usage: InputHelper::read(variable) returns bool
   ========================================================= */
public:
    static void clear()
    {
        /**
         * Clears cin error state and flushes input buffer
         *
         * Called automatically when input fails to prevent
         */
        cin.clear();             // Reset error state flags
        cin.ignore(10000, '\n'); // Flush buffer up to newline or 10000 chars
    }

    template <typename T>
    static bool read(T &value)
    {
        /**
         * Template method for type-safe input reading
         *
         * @param value Reference to variable to store input
         * @return true if input succeeded, false if type mismatch occurred
         *
         * This template allows the same function to work with
         * int, float, double, etc. without code duplication
         */
        cin >> value;   // Attempt to read input
        if (cin.fail()) // Check if extraction failed
        {
            clear();      // Clean up error state
            return false; // Signal failure to caller
        }
        return true; // Input successful
    }
};

struct Address
{
    /* =========================================================
    Address Structure

    Purpose: Encapsulates location/address information

    Type: Plain Old Data (POD) struct

    Design Choice: Using struct instead of class because
    all members are public and no behavior is needed
   ========================================================= */
    string city;   // City name
    string street; // Street address
};

class Person
{
    /* =========================================================
    Person Class (Abstract Base Class)

    Purpose: Defines common interface for all university members

    Design Pattern: Abstract base class for polymorphism

    OOP Concepts Demonstrated:
    - Abstraction (pure virtual functions)
    - Encapsulation (protected members)
    - Inheritance (base for Student and Professor)
    - Polymorphism (virtual methods)
   ========================================================= */
protected:
    string name;
    int id;
    Address address; // Composition: Person "has-a" Address

public:
    Person() : id(0), name("Unknown") {}

    virtual ~Person() = default;
    /**
     * Virtual destructor (required for polymorphic base classes)
     *
     * Ensures derived class destructors are called properly
     * when deleting through base class pointer
     */

    virtual void display() const = 0;

    virtual void save(ofstream &out) const = 0;
    /**
     * Pure virtual function for saving to file
     *
     * @param out Output file stream reference
     */

    void setName(string n) { name = n; }
};

class Student : public Person
{
    /* =========================================================
    Student Class

    Purpose: Represents a student entity in the system

    Inheritance: Derives from Person (is-a relationship)

    OOP Concepts Demonstrated:
    - Inheritance (extends Person)
    - Method overriding (display, save)
    - Method overloading (updateInfo)
    - Static members (count)
    - Arrays (lastTestScores)
    - Exception handling (GPA validation)
   ========================================================= */
private:
    float gpa;               // Grade Point Average (0.0 - 4.0)
    float lastTestScores[3]; // Array: stores last 3 test scores
    static int count;        // Static: tracks total Student instances

public:
    Student()
    {
        gpa = 0.0;
        for (int i = 0; i < 3; i++)
            lastTestScores[i] = 0.0; // Initialize array elements
        count++;                     // Increment static counter
    }

    Student(const string &n, int i, const Address &addr, float g)
    {
        // Input validation with exception throwing
        if (g < 0.0f || g > 4.0f)
            throw runtime_error("GPA must be between 0.0 and 4.0");

        name = n;
        id = i;
        address = addr;
        gpa = g;

        // Initialize array with default test scores
        for (int k = 0; k < 3; k++)
            lastTestScores[k] = 10.0;

        count++; // Track instance count
    }

    ~Student()
    {
        count--; // Decrements static counter when student object is destroyed
    }

    void display() const override
    {
        /**
         * Overrides Person::display() - Polymorphism
         *
         * Provides Student-specific implementation of display
         * Can be called through Person* pointer (runtime polymorphism)
         */
        cout << "[Student] " << id
             << " | " << name
             << " | GPA: " << gpa
             << " | City: " << address.city << endl;
    }

    void save(ofstream &out) const override
    {
        /**
         * Overrides Person::save() - Polymorphism
         *
         * Writes student data to file stream
         * Uses fstream for file I/O operations
         *
         * @param out Output file stream (passed by reference)
         */
        out << "Student | " << id
            << " | " << name
            << " | GPA: " << gpa
            << " | City: " << address.city << endl;
    }

    void updateInfo(const string &newName)
    {
        /**
         * Overloaded method #1 - Updates student name
         *
         * @param newName New name to assign
         *
         * Method Overloading: Same name, different parameters
         */
        this->name = newName; // 'this' pointer refers to current object
        cout << "Name updated.\n";
    }

    void updateInfo(const Address &newAddr)
    {
        /**
         * Overloaded method #2 - Updates student address
         *
         * @param newAddr New address to assign
         *
         * Demonstrates method overloading with different parameter type
         */
        this->address = newAddr;
        cout << "Address updated.\n";
    }

    static int getCount()
    {
        /**
         * Static member function
         *
         * @return Current count of Student instances
         *
         * Can be called without an object: Student::getCount()
         * Accesses static member variable
         */
        return count;
    }
};

// Static member initialization (must be done outside class)
int Student::count = 0;

class Professor : public Person
{
    /* =========================================================
    Professor Class

    Purpose: Represents a professor entity in the system

    Inheritance: Derives from Person (is-a relationship)

    OOP Concepts Demonstrated:
    - Inheritance
    - Method overriding
    - Polymorphism
   ========================================================= */
private:
    double salary;

public:
    Professor()
    {
        /**
         * Default constructor
         *
         * Initializes salary to 0.0
         */
        salary = 0.0;
    }

    Professor(const string &n, int i, const Address &addr, double s)
    {
        /**
         * Parameterized constructor
         *
         * @param n Professor name
         * @param i Professor ID
         * @param addr Professor address
         * @param s Salary amount
         */
        name = n; // Access protected member from base class
        id = i;
        address = addr;
        salary = s;
    }

    void display() const override
    {
        /**
         * Overrides Person::display()
         *
         * Provides Professor-specific display implementation
         * Demonstrates runtime polymorphism
         */
        cout << "[Professor] " << id
             << " | " << name
             << " | Salary: $" << salary << endl;
    }

    void save(ofstream &out) const override
    {
        /**
         * Overrides Person::save()
         *
         * Writes professor data to file
         *
         * @param out Output file stream
         */
        out << "Professor | " << id
            << " | " << name
            << " | Salary: $" << salary
            << " | City: " << address.city << endl;
    }
};

template <typename T>
class UniversityRepository
{
    /* =========================================================
    UniversityRepository Template Class

    Purpose: Generic container for managing entity collections

    Design Pattern: Repository Pattern

    Template Parameter: T - Type of entity to store

    OOP Concepts Demonstrated:
    - Templates (generic programming)
    - Pointers (dynamic memory management)
    - Vector
   ========================================================= */
private:
    vector<T *> records; // Vector of pointers for polymorphic storage

public:
    void add(T *obj)
    {
        /**
         * Adds an entity to the repository
         *
         * @param obj Pointer to dynamically allocated object
         *
         * Repository takes ownership of the pointer
         * and is responsible for deletion
         */
        records.push_back(obj);
    }

    void displayAll() const
    {
        /**
         * Displays all records in the repository
         *
         * Uses polymorphism - calls the appropriate display()
         * method based on actual object type (Student or Professor)
         */
        if (records.empty()) // Check if repository is empty
        {
            cout << "No records found.\n";
            return;
        }

        // 'auto' deduces type of the elemnt in the record (for polymorhisim)
        for (auto r : records)
            r->display(); // Polymorphic call through pointer
    }

    const vector<T *> &getAll() const
    {
        /**
         * Returns const reference to internal vector
         *
         * @return Read-only reference to records
         *
         * Returns const to prevent external modification
         * Avoids copying the entire vector (efficient)
         */

        return records;
    }

    ~UniversityRepository()
    {
        /**
         * Destructor - Implements RAII principle
         *
         * Automatically cleans up all dynamically allocated objects
         * Prevents memory leaks by deleting all pointers
         */
        for (auto r : records)
            delete r; // Free each dynamically allocated object
    }
};

template <typename T>
class FileService
{
    /* =========================================================
    FileService Template Class

    Purpose: Handles file I/O operations for persistence

    Design Pattern: Service/Utility class with static methods

    Template Parameter: T - Type of entity to save

    OOP Concepts Demonstrated:
    - Templates
    - Static methods
    - File stream operations (fstream)
    - Separation of concerns
   ========================================================= */
public:
    static void save(const string &filename, const vector<T *> &data)
    {
        /**
         * Saves collection of entities to a text file
         *
         * @param filename Name of file to create/overwrite
         * @param data Vector of entity pointers to save
         *
         * Static method - can be called without creating
         * an instance of FileService
         *
         * Uses polymorphism to call appropriate save() method
         */
        ofstream out(filename); // Create/open file for writing

        // Error handling - check if file opened successfully
        if (!out)
        {
            cout << "File error.\n";
            return;
        }

        // Write each object to file
        // 'auto' deduced as T* (pointer type)
        for (auto obj : data)
            obj->save(out); // Polymorphic call to save()

        out.close(); // Close file stream
        cout << "Saved to " << filename << endl;
    }
};

class Menu
{
    /* =========================================================
    Menu Class

    Purpose: Manages user interface and application flow

    Design Pattern: Facade pattern (simplifies complex operations)

    OOP Concepts Demonstrated:
    - Composition (has-a relationship with repositories)
    - Exception handling
    - Encapsulation (private helper methods)
   ========================================================= */
private:
    // Composition: Menu "has-a" repository for students
    UniversityRepository<Student> students;

    // Composition: Menu "has-a" repository for professors
    UniversityRepository<Professor> professors;

public:
    void run()
    {
        /**
         * Main application loop
         *
         * Displays main menu and routes user to appropriate submenu
         * Continues until user chooses to exit
         */
        int choice;
        do
        {
            // Display main menu options
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Students\n2. Professors\n3. Exit\nChoice: ";

            // Validate numeric input
            if (!InputHelper::read(choice))
                continue; // Skip invalid input and re-prompt

            // Route to appropriate submenu
            if (choice == 1)
                studentMenu();
            else if (choice == 2)
                professorMenu();

        } while (choice != 3); // Loop until user exits
    }

private:
    void studentMenu()
    {
        /**
         * Student management submenu
         *
         * Provides operations for adding, viewing, and saving students
         * Includes exception handling for validation errors
         */
        int choice;
        do
        {
            // Display student menu options
            cout << "\n--- Student Menu ---\n";
            cout << "1. Add\n2. View\n3. Save\n4. Back\nChoice: ";

            if (!InputHelper::read(choice))
                continue;

            // Try-catch block for exception handling
            try
            {
                if (choice == 1)
                    addStudent(); // Add new student
                else if (choice == 2)
                    students.displayAll(); // View all students
                else if (choice == 3)
                    FileService<Student>::save("students.txt", students.getAll());
            }

            catch (const exception &e) // Catch any exception (polymorphic catch USING POINTER)
            {
                cout << e.what() << endl; // Display error message
            }

        } while (choice != 4); // Loop until user goes back
    }

    void professorMenu()
    {
        /**
         * Professor management submenu
         *
         * Provides operations for adding, viewing, and saving professors
         */
        int choice;
        do
        {
            // Display professor menu options
            cout << "\n--- Professor Menu ---\n";
            cout << "1. Add\n2. View\n3. Save\n4. Back\nChoice: ";

            if (!InputHelper::read(choice))
                continue;

            // Route to appropriate function
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
        /**
         * Collects student information from user and creates Student object
         *
         * Demonstrates:
         * - Input validation using InputHelper
         * - Dynamic memory allocation (new operator)
         * - Exception propagation to caller
         */
        string name;
        Address addr;
        int id;
        float gpa;

        // Collect student information
        cout << "Name: ";
        cin >> name;

        cout << "ID: ";
        if (!InputHelper::read(id)) // Validate integer input
            return;                 // Exit if invalid

        cout << "City: ";
        cin >> addr.city;

        cout << "Street: ";
        cin >> addr.street;

        cout << "GPA: ";
        if (!InputHelper::read(gpa)) // Validate float input
            return;

        // Create new Student object dynamically
        // Constructor may throw exception if GPA is invalid
        students.add(new Student(name, id, addr, gpa));
    }

    void addProfessor()
    {
        /**
         * Collects professor information from user and creates Professor object
         *
         * Similar to addStudent but for Professor entities
         */
        string name;
        Address addr;
        int id;
        double salary;

        // Collect professor information
        cout << "Name: ";
        cin >> name;

        cout << "ID: ";
        if (!InputHelper::read(id))
            return; // Exit if invalid

        cout << "City: ";
        cin >> addr.city;

        cout << "Street: ";
        cin >> addr.street;

        cout << "Salary: ";
        if (!InputHelper::read(salary)) // Validate double input
            return;                     // Exit if invalid

        // Create new Professor object dynamically
        professors.add(new Professor(name, id, addr, salary));
    }
};

int main()
{
    /* =========================================================
    Main Function

    Entry point of the application

    Creates Menu object and starts the application loop
    When Menu object goes out of scope, its destructor
    is called, which triggers repository destructors,
    which delete all Student and Professor objects
    (RAII principle ensures automatic cleanup)
   ========================================================= */

    Menu app;  // Create menu object on stack
    app.run(); // Start application
    return 0;
} // app destroyed here, triggering cleanup