#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

/* ===================== INPUT HELPER ===================== */
class InputHelper
{
public:
    static void clear()
    {
        cin.clear();
        cin.ignore(10000, '\n');
    }

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

/* ===================== ADDRESS ===================== */
struct Address
{
    string city;
    string street;
};

/* ===================== PERSON (ABSTRACT) ===================== */
class Person
{
protected:
    string name;
    int id;
    Address address;

public:
    virtual ~Person() = default;
    virtual void display() const = 0;
    virtual void save(ofstream &out) const = 0;
};

/* ===================== PATIENT ===================== */
class Patient : public Person
{
private:
    string wardType; // e.g., General, ICU, VIP
    static int count;

public:
    Patient(const string &n, int i, const Address &addr, const string &ward)
    {
        if (ward != "General" && ward != "ICU" && ward != "VIP")
            throw runtime_error("Ward type must be General, ICU, or VIP");

        name = n;
        id = i;
        address = addr;
        wardType = ward;
        count++;
    }

    ~Patient() { count--; }

    void display() const override
    {
        cout << "[Patient] " << id
             << " | " << name
             << " | Ward: " << wardType << endl;
    }

    void save(ofstream &out) const override
    {
        out << "Patient | " << id
            << " | " << name
            << " | Ward: " << wardType
            << " | City: " << address.city << endl;
    }

    static int getCount() { return count; }
};

int Patient::count = 0;

/* ===================== DOCTOR ===================== */
class Doctor : public Person
{
private:
    double salary;

public:
    Doctor(const string &n, int i, const Address &addr, double s)
    {
        name = n;
        id = i;
        address = addr;
        salary = s;
    }

    void display() const override
    {
        cout << "[Doctor] " << id
             << " | " << name
             << " | Salary: $" << salary << endl;
    }

    void save(ofstream &out) const override
    {
        out << "Doctor | " << id
            << " | " << name
            << " | Salary: $" << salary
            << " | City: " << address.city << endl;
    }
};

/* ===================== HOSPITAL REPOSITORY ===================== */
template <typename T>
class HospitalRepository
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

    const vector<T *> &getAll() const
    {
        return records;
    }

    ~HospitalRepository()
    {
        for (auto r : records)
            delete r;
    }
};

/* ===================== FILE SERVICE ===================== */
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

/* ===================== MENU ===================== */
class Menu
{
private:
    HospitalRepository<Patient> patients;
    HospitalRepository<Doctor> doctors;

public:
    void run()
    {
        int choice;
        do
        {
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Patients\n2. Doctors\n3. Exit\nChoice: ";
            if (!InputHelper::read(choice))
                continue;

            if (choice == 1)
                patientMenu();
            else if (choice == 2)
                doctorMenu();

        } while (choice != 3);
    }

private:
    void patientMenu()
    {
        int choice;
        do
        {
            cout << "\n--- Patient Menu ---\n";
            cout << "1. Add Patient\n2. View Patients\n3. Save Patients\n4. Back\nChoice: ";
            if (!InputHelper::read(choice))
                continue;

            try
            {
                if (choice == 1)
                    addPatient();
                else if (choice == 2)
                    patients.displayAll();
                else if (choice == 3)
                    FileService<Patient>::save("patients.txt", patients.getAll());
            }
            catch (const exception &e)
            {
                cout << "Error: " << e.what() << endl;
            }

        } while (choice != 4);
    }

    void doctorMenu()
    {
        int choice;
        do
        {
            cout << "\n--- Doctor Menu ---\n";
            cout << "1. Add Doctor\n2. View Doctors\n3. Save Doctors\n4. Back\nChoice: ";
            if (!InputHelper::read(choice))
                continue;

            if (choice == 1)
                addDoctor();
            else if (choice == 2)
                doctors.displayAll();
            else if (choice == 3)
                FileService<Doctor>::save("doctors.txt", doctors.getAll());

        } while (choice != 4);
    }

    void addPatient()
    {
        string name, ward;
        Address addr;
        int id;

        cout << "Name: ";
        cin >> name;
        cout << "ID: ";
        if (!InputHelper::read(id))
            return;
        cout << "City: ";
        cin >> addr.city;
        cout << "Street: ";
        cin >> addr.street;
        cout << "Ward Type (General/ICU/VIP): ";
        cin >> ward;

        patients.add(new Patient(name, id, addr, ward));
    }

    void addDoctor()
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

        doctors.add(new Doctor(name, id, addr, salary));
    }
};

/* ===================== MAIN ===================== */
int main()
{
    Menu app;
    app.run();
    return 0;
}
