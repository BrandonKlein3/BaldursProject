// Comment this out to run full program instead of tests
#define RUN_TESTS

#ifdef RUN_TESTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN //only works while in debug
#include "doctest.h"
#endif

#ifndef RUN_TESTS
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>

using namespace std;

// Goal: Track player character and multiple play sessions while practicing C++ fundamentals

// Constants
// Avoids magic numbers and improves readability.
const int MAX_SESSIONS = 10;
const int MIN_LEVEL = 1;
const int MAX_LEVEL = 12;
const int MAX_ENEMIES = 1000;


// Enum represents the possible difficulty recommendation that will be used later in switch
// statements and conditional logic
enum Difficulty {
    EXPLORER = 1,
    BALANCED,
    TACTICIAN
};

// Character Sheet
// Stores persistant data
struct Character {
    string name;
    int level;
    double gold;
    Difficulty difficulty;
};

// Validation Functions


int getValidInt(string prompt, int min, int max) {
    int value;

    while (true) {
        cout << prompt;

        if (!(cin >> value)) {
            cout << "Invalid input. Enter a NUMBER.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (value < min || value > max) {
            cout << "Out of range (" << min << "-" << max << "). Try again.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}

string getValidString(string prompt) {
    string input;

    while (true) {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "Input cannot be empty. Try again.\n";
            continue;
        }

        return input;
    }
}

// Create Character Sheet Function
// Collects user input, validates and stores it in a character struct passed by reference
void createCharacter(Character& c) {

    // Get character data
    c.name = getValidString("Enter character name: ");
    c.level = getValidInt("Enter level (1-12): ", MIN_LEVEL, MAX_LEVEL);
    c.gold = getValidInt("Enter starting gold: ", 0, 100000);
    c.difficulty = BALANCED;
}


// View Character Sheet Summary Function

void displayCharacterSummary(const Character& c) {
    cout << "\n=== Character Summary ===\n";
    cout << "Name: " << c.name << endl;
    cout << "Level: " << c.level << endl;
    cout << "Gold: " << c.gold << endl;
}

// Recommend difficulty by stats
Difficulty recommendDifficultyByStats(int level, double hours) {
    if (level < 4 || hours < 1.5) return EXPLORER;
    if (level < 8) return BALANCED;
    return TACTICIAN;
}

// Class for play sessions BASE CLASS
class PlaySession {
protected:
    string location;
    int durationMinutes;
    Difficulty difficulty;

public:
    PlaySession() : location("Unknown"), durationMinutes(0), difficulty(EXPLORER) {}

    PlaySession(const string& loc, int duration, Difficulty diff)
        : location(loc), durationMinutes(duration), difficulty(diff) {}

    string getLocation() const { return location; }
    int getDuration() const { return durationMinutes; }

    virtual double calculateValue() const = 0;

    virtual void print() const {
        cout << "Location: " << location << endl;
        cout << "Duration: " << durationMinutes << endl;
    }

    virtual ~PlaySession() {}
};

// Class for loot info COMPOSITION CLASS
class LootInfo {
    int goldEarned;
    bool rareItemFound;

public:
    LootInfo(int g = 0, bool r = false) : goldEarned(g), rareItemFound(r) {}
};

// DERIVED CLASS Combat Session
class CombatSession : public PlaySession {
    int enemiesDefeated;
    LootInfo loot;

public:
    CombatSession() : enemiesDefeated(0) {}

    CombatSession(const string& loc, int dur, Difficulty diff,
        int enemies, const LootInfo& l)
        : PlaySession(loc, dur, diff), enemiesDefeated(enemies), loot(l) {}

    int getEnemiesDefeated() const { return enemiesDefeated; }

    double calculateValue() const override {
        return enemiesDefeated * 10.0;
    }

    bool operator==(const CombatSession& o) const {
        return location == o.location &&
            durationMinutes == o.durationMinutes &&
            enemiesDefeated == o.enemiesDefeated;
    }

    // Override toStream in derived class
    friend ostream& operator<<(ostream& os, const CombatSession& cs) {
        os << "Combat at " << cs.location << " | Enemies: " << cs.enemiesDefeated;
        return os;
    }
};


// DERIVED CLASS ExplorationSession
class ExplorationSession : public PlaySession {
    int areasDiscovered;
    LootInfo loot;    // composition

public:
    ExplorationSession() : areasDiscovered(0) {}

    ExplorationSession(const string& loc, int dur, Difficulty diff,
        int areas, const LootInfo& l)
        : PlaySession(loc, dur, diff), areasDiscovered(areas), loot(l) {}

    int getAreasDiscovered() const { return areasDiscovered; }

    double calculateValue() const override {
        return areasDiscovered * 5.0;
    }
};

// Exception Class
class ContainerException : public runtime_error {
public:
    ContainerException(const string& msg) : runtime_error(msg) {}
};

// ================= LINKED LIST =================
class SessionLinkedList {
public:
    struct Node {
        PlaySession* data;
        Node* next;
        Node(PlaySession* d) : data(d), next(nullptr) {}
    };

    Node* head = nullptr;

    void insertBack(PlaySession* s) {
        Node* n = new Node(s);
        if (!head) { head = n; return; }

        Node* t = head;
        while (t->next) t = t->next;
        t->next = n;
    }

    int size() {
        int c = 0;
        for (Node* t = head; t; t = t->next) c++;
        return c;
    }

    PlaySession* at(int index) {
        Node* t = head;
        for (int i = 0; t; t = t->next, i++)
            if (i == index) return t->data;
        return nullptr;
    }
};


class ListIterator {
    SessionLinkedList::Node* current;
public:
    ListIterator(SessionLinkedList::Node* start) : current(start) {}
    bool hasNext() { return current != nullptr; }
    void next() { current = current->next; }
    PlaySession* getData() { return current->data; }
};


// Session Container replaces old template class
class SessionContainer {
    SessionLinkedList list;

public:
    void add(PlaySession* s) { list.insertBack(s); }
    int size() { return list.size(); }

    PlaySession* at(int index) {
        PlaySession* r = list.at(index);
        if (!r) throw ContainerException("Index out of bounds");
        return r;
    }

    SessionLinkedList::Node* getHead() { return list.head; }

    // -------- LINEAR SEARCH --------
    int linearSearch(const string& loc) {
        for (int i = 0; i < size(); i++)
            if (at(i)->getLocation() == loc) return i;
        return -1;
    }

    void remove(int index) {
        if (index < 0 || index >= size())
            throw ContainerException("Invalid index");

        auto* curr = list.head;
        SessionLinkedList::Node* prev = nullptr;

        for (int i = 0; i < index; i++) {
            prev = curr;
            curr = curr->next;
        }

        if (prev) prev->next = curr->next;
        else list.head = curr->next;

        delete curr->data;
        delete curr;
    }
};

// ================= STACK =================
class SessionStack {
    SessionLinkedList list;

public:
    void push(PlaySession* s) { list.insertBack(s); }

    void pop() {
        if (list.size() == 0) throw ContainerException("Stack empty");

        auto* curr = list.head;
        SessionLinkedList::Node* prev = nullptr;

        while (curr->next) {
            prev = curr;
            curr = curr->next;
        }

        if (prev) prev->next = nullptr;
        else list.head = nullptr;

        delete curr->data;
        delete curr;
    }

    PlaySession* top() { return list.at(list.size() - 1); }
    bool isEmpty() { return list.size() == 0; }
};

// ================= QUEUE =================
class SessionQueue {
    SessionLinkedList list;

public:
    void enqueue(PlaySession* s) { list.insertBack(s); }

    void dequeue() {
        if (list.size() == 0) throw ContainerException("Queue empty");

        auto* temp = list.head;
        list.head = list.head->next;

        delete temp->data;
        delete temp;
    }

    PlaySession* front() { return list.at(0); }
    bool isEmpty() { return list.size() == 0; }
};



// Banner Function
void displayBanner() {
    cout << "\n=== Baldur's Gate 3 - Adventure Tracker ===\n";
}

// Menu Display Function
void displayMenu() {
    cout << "\n=== Main Menu ===\n1. Add Session\n2. View Session Summary\n3. Remove Session\n4. Recommend Difficulty\n5. Save Report to File\n6. Quit\n7. Search by Location\n8. Push to stack\n9. Pop from stack\n10. Enqueue to queue\n11. Dequeue from queue\n";
}

#ifndef RUN_TESTS
int main() {

    Character player;
    SessionContainer manager;
    SessionStack stack;
    SessionQueue queue;
    int choice;

    displayBanner();
    createCharacter(player);
    displayCharacterSummary(player);

    do {
        displayMenu();
        choice = getValidInt("Choice: ", 1, 11);

        switch (choice) {

        case 1:   // Add Session
        {
            int type = getValidInt("1 Combat, 2 Explore: ", 1, 2);
            string loc = getValidString("Location: ");
            int dur = getValidInt("Duration: ", 1, 600);

            LootInfo loot(0, false);

            if (type == 1)
                manager.add(new CombatSession(loc, dur, BALANCED, 5, loot));
            else
                manager.add(new ExplorationSession(loc, dur, EXPLORER, 3, loot));

            cout << "Added\n";
            break;
        }

        case 2:   // View Sessions
        {
            ListIterator it(manager.getHead());
            while (it.hasNext()) {
                it.getData()->print();
                cout << "---\n";
                it.next();
            }
            break;
        }

        case 3:   // Remove Session
        {
            if (manager.size() == 0) {
                cout << "No sessions to remove.\n";
                break;
            }

            int index = getValidInt(
                "Enter session index to remove: ",
                0,
                manager.size() - 1
            );

            try {
                manager.remove(index);       // Remove session by index
                cout << "Session removed.\n";
            }
            catch (const ContainerException& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }

        case 4:   // Recommend Difficulty
        {
            if (manager.size() == 0) {
                cout << "No sessions available.\n";
                break;
            }

            int totalMinutes = 0;
            for (int i = 0; i < manager.size(); i++) {
                totalMinutes += manager.at(i)->getDuration();
            }

            double avgHours =
                (totalMinutes / 60.0) / manager.size();

            Difficulty rec =
                recommendDifficultyByStats(player.level, avgHours);

            cout << "\n=== Difficulty Recommendation ===\n";

            switch (rec) {
            case EXPLORER:  cout << "Explorer\n"; break;
            case BALANCED:  cout << "Balanced\n"; break;
            case TACTICIAN: cout << "Tactician\n"; break;
            }

            break;
        }

        case 5:   // Save Report
        {
            ofstream outFile("report.txt");

            outFile << "Adventure Report\n\n";
            outFile << "Character: " << player.name << endl;
            outFile << "Level: " << player.level << endl;
            outFile << fixed << setprecision(2);
            outFile << "Gold: " << player.gold << "\n\n";

            for (int i = 0; i < manager.size(); i++) {
                outFile << "Session #" << i << ":\n";
                manager.at(i)->print();
                outFile << "\n";
            }

            outFile.close();
            cout << "Report saved to report.txt\n";
            break;
        }

        case 6:   // Quit
        {
            cout << "Exiting Adventure Tracker. Goodbye!\n";
            return 0;
        }
        case 7:   // Enter location    
        {
            string loc = getValidString("Enter location: ");
            int index = manager.linearSearch(loc);

            if (index != -1)
                cout << "Found at index: " << index << endl;
            else
                cout << "Not found.\n";

            break;
        }
    
        case 8:   // Push to stack   
            stack.push(new CombatSession("Camp", 30, BALANCED, 5, LootInfo()));
            break;

        case 9:   // Pop from stack
            try { stack.pop(); } catch (...) { cout << "Empty\n"; }
            break;

        case 10:  // Enqueue to queue and Dequeue from queue
            queue.enqueue(new ExplorationSession("Forest", 60, EXPLORER, 3, LootInfo()));
            break;
            try { queue.dequeue(); } catch (...) { cout << "Empty\n"; }
            break;
        }

    } while (choice != 6);

    #ifdef _DEBUG
        _CrtDumpMemoryLeaks();
    #endif

    return 0;
}
#endif

// ===================== UNIT TESTS =====================
#ifdef RUN_TESTS

// ---------- A) Polymorphism ----------
TEST_CASE("calculateValue works polymorphically") {
	LootInfo loot(0, false);
	PlaySession* c = new CombatSession("Camp", 30, BALANCED, 5, loot);
	PlaySession* e = new ExplorationSession("Forest", 60, EXPLORER, 4, loot);

	CHECK(c->calculateValue() == 50.0);
	CHECK(e->calculateValue() == 20.0);

	delete c;
	delete e;
}

// ---------- B) Constructors ----------
TEST_CASE("CombatSession initializes correctly") {
	LootInfo loot(100, true);
	CombatSession cs("Ruins", 45, TACTICIAN, 8, loot);

	CHECK(cs.getLocation() == "Ruins");
	CHECK(cs.getEnemiesDefeated() == 8);
}

// ---------- C) Equality Operator ----------
TEST_CASE("CombatSession operator== works") {
	LootInfo loot(0, false);
	CombatSession a("Camp", 30, BALANCED, 5, loot);
	CombatSession b("Camp", 30, BALANCED, 5, loot);
	CHECK(a == b);
}

// ---------- D) Stream Operator ----------
TEST_CASE("operator<< outputs correct string") {
	LootInfo loot(0, false);
	CombatSession cs("Camp", 30, BALANCED, 5, loot);
	ostringstream oss;
	oss << cs;
	CHECK(oss.str() == "Combat at Camp | Enemies: 5");
}

// ---------- E) Container ----------
TEST_CASE("SessionContainer works") {
	SessionContainer m;
	LootInfo loot(0, false);
	m.add(new CombatSession("Camp", 30, BALANCED, 5, loot));
	CHECK(m.size() == 1);
}

// ---------- F) Exceptions ----------
TEST_CASE("Container throws") {
	SessionContainer m;
	CHECK_THROWS(m.at(0));
}

// ---------- G) Linear Search ----------
TEST_CASE("Search works") {
	SessionContainer m;
	LootInfo loot(0, false);
	m.add(new CombatSession("Camp", 30, BALANCED, 5, loot));
	CHECK(m.linearSearch("Camp") == 0);
}

// ---------- H) Template ----------
template<typename T>
T addValues(T a, T b) { return a + b; }

TEST_CASE("template works") {
	CHECK(addValues(2, 3) == 5);
}

// ---------- I) Stack ----------
TEST_CASE("Stack operations") {
	SessionStack s;
	LootInfo loot(0, false);

	s.push(new CombatSession("Camp", 30, BALANCED, 5, loot));
	CHECK_FALSE(s.isEmpty());
	CHECK(s.top()->getLocation() == "Camp");

	s.pop();
	CHECK(s.isEmpty());

	CHECK_THROWS(s.pop());
}

// ---------- J) Queue ----------
TEST_CASE("Queue operations") {
    SessionQueue q;
    LootInfo loot(0, false);

    q.enqueue(new ExplorationSession("Forest", 60, EXPLORER, 3, loot));
    CHECK_FALSE(q.isEmpty());
    CHECK(q.front()->getLocation() == "Forest");

    q.dequeue();
    CHECK(q.isEmpty());

    CHECK_THROWS(q.dequeue());
}
#endif


