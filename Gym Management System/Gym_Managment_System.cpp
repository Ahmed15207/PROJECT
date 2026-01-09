#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

/* ===================== INPUT HELPER ===================== */
class InputHelper {
public:
    static void clear() {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    template<typename T>
    static bool read(T& value) {
        cin >> value;
        if (cin.fail()) {
            clear();
            return false;
        }
        return true;
    }
};

/* ===================== ADDRESS ===================== */
struct Address {
    string city;
    string street;
};

/* ===================== MEMBER (ABSTRACT) ===================== */
class Member {
protected:
    string name;
    int id;
    Address address;

public:
    virtual ~Member() = default;
    virtual void display() const = 0;
    virtual void save(ofstream& out) const = 0;
};

/* ===================== GYMMEMBER ===================== */
class GymMember : public Member {
private:
    int membershipLevel;  // 1-Basic, 2-Premium, 3-VIP
    static int count;

public:
    GymMember(const string& n, int i, const Address& addr, int level) {
        if (level < 1 || level > 3)
            throw runtime_error("Membership level must be 1 (Basic), 2 (Premium), or 3 (VIP)");

        name = n;
        id = i;
        address = addr;
        membershipLevel = level;
        count++;
    }

    ~GymMember() { count--; }

    void display() const override {
        cout << "[GymMember] " << id
             << " | " << name
             << " | Membership Level: " << membershipLevel << endl;
    }

    void save(ofstream& out) const override {
        out << "GymMember | " << id
            << " | " << name
            << " | Level: " << membershipLevel
            << " | City: " << address.city << endl;
    }

    static int getCount() { return count; }
};

int GymMember::count = 0;

/* ===================== TRAINER ===================== */
class Trainer : public Member {
private:
    double salary;

public:
    Trainer(const string& n, int i, const Address& addr, double s) {
        name = n;
        id = i;
        address = addr;
        salary = s;
    }

    void display() const override {
        cout << "[Trainer] " << id
             << " | " << name
             << " | Salary: $" << salary << endl;
    }

    void save(ofstream& out) const override {
        out << "Trainer | " << id
            << " | " << name
            << " | Salary: $" << salary
            << " | City: " << address.city << endl;
    }
};

/* ===================== GYM REPOSITORY ===================== */
template<typename T>
class GymRepository {
private:
    vector<T*> records;

public:
    void add(T* obj) {
        records.push_back(obj);
    }

    void displayAll() const {
        if (records.empty()) {
            cout << "No records found.\n";
            return;
        }
        for (auto r : records) r->display();
    }

    const vector<T*>& getAll() const {
        return records;
    }

    ~GymRepository() {
        for (auto r : records) delete r;
    }
};

/* ===================== FILE SERVICE ===================== */
template<typename T>
class FileService {
public:
    static void save(const string& filename, const vector<T*>& data) {
        ofstream out(filename);
        if (!out) {
            cout << "File error.\n";
            return;
        }
        for (auto obj : data) obj->save(out);
        out.close();
        cout << "Saved to " << filename << endl;
    }
};

/* ===================== MENU ===================== */
class Menu {
private:
    GymRepository<GymMember> members;
    GymRepository<Trainer> trainers;

public:
    void run() {
        int choice;
        do {
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Gym Members\n2. Trainers\n3. Exit\nChoice: ";
            if (!InputHelper::read(choice)) continue;

            if (choice == 1) memberMenu();
            else if (choice == 2) trainerMenu();

        } while (choice != 3);
    }

private:
    void memberMenu() {
        int choice;
        do {
            cout << "\n--- Gym Member Menu ---\n";
            cout << "1. Add Member\n2. View Members\n3. Save Members\n4. Back\nChoice: ";
            if (!InputHelper::read(choice)) continue;

            try {
                if (choice == 1) addMember();
                else if (choice == 2) members.displayAll();
                else if (choice == 3)
                    FileService<GymMember>::save("members.txt", members.getAll());
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }

        } while (choice != 4);
    }

    void trainerMenu() {
        int choice;
        do {
            cout << "\n--- Trainer Menu ---\n";
            cout << "1. Add Trainer\n2. View Trainers\n3. Save Trainers\n4. Back\nChoice: ";
            if (!InputHelper::read(choice)) continue;

            if (choice == 1) addTrainer();
            else if (choice == 2) trainers.displayAll();
            else if (choice == 3)
                FileService<Trainer>::save("trainers.txt", trainers.getAll());

        } while (choice != 4);
    }

    void addMember() {
        string name;
        Address addr;
        int id, level;

        cout << "Name: "; cin >> name;
        cout << "ID: "; if (!InputHelper::read(id)) return;
        cout << "City: "; cin >> addr.city;
        cout << "Street: "; cin >> addr.street;
        cout << "Membership Level (1-Basic,2-Premium,3-VIP): "; if (!InputHelper::read(level)) return;

        members.add(new GymMember(name, id, addr, level));
    }

    void addTrainer() {
        string name;
        Address addr;
        int id;
        double salary;

        cout << "Name: "; cin >> name;
        cout << "ID: "; if (!InputHelper::read(id)) return;
        cout << "City: "; cin >> addr.city;
        cout << "Street: "; cin >> addr.street;
        cout << "Salary: "; if (!InputHelper::read(salary)) return;

        trainers.add(new Trainer(name, id, addr, salary));
    }
};

/* ===================== MAIN ===================== */
int main() {
    Menu app;
    app.run();
    return 0;
}
