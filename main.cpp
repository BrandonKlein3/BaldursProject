#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN //only works while in debug
#include "doctest.h"
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// Goal: Track player character and multiple play sessions while practicing C++ fundamentals

// Constants
// Avoids magic numbers and improves readability.
const int MAX_SESSIONS = 10;
const int MIN_LEVEL = 1;
const int MAX_LEVEL = 12;
const int MAX_ENEMIES = 1000;


// Enum represents the possible difficuly recommendation that will be used later in switch
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

// Class for play sessions BASE CLASS
class PlaySession {
protected:
	string location;
	int durationMinutes;
	Difficulty difficulty;

public:
	PlaySession()
		: location("Unknown"), durationMinutes(0), difficulty(EXPLORER) {
	}

	PlaySession(const string& loc, int duration, Difficulty diff)
		: location(loc), durationMinutes(duration), difficulty(diff) {
	}

	string getLocation() const { return location; }
	int getDuration() const { return durationMinutes; }
	Difficulty getDifficulty() const { return difficulty; }

	void setLocation(const string& loc) { location = loc; }
	void setDuration(int duration) { durationMinutes = duration; }
	void setDifficulty(Difficulty diff) { difficulty = diff; }

	virtual double calculateValue() const = 0;

	virtual void print() const {
		cout << "Location: " << location << endl;
		cout << "Duration (minutes): " << durationMinutes << endl;
	}

	// Virtual toStream()
	virtual void toStream(std::ostream& os) const {
		os << location << " | Duration: " << durationMinutes;
	}

	virtual ~PlaySession() {}

	
};


int calculateTotalDuration(const PlaySession* const sessions[], int count);
int calculateTotalEnemies(const PlaySession* const sessions[], int count);


// Class for loot info COMPOSITION CLASS
class LootInfo {
private:
	int goldEarned;
	bool rareItemFound;

public:
	LootInfo() : goldEarned(0), rareItemFound(false) {}
	LootInfo(int gold, bool rare)
		: goldEarned(gold), rareItemFound(rare) {
	}

	int getGoldEarned() const { return goldEarned; }
	bool hasRareItem() const { return rareItemFound; }

	void setGoldEarned(int gold) { goldEarned = gold; }
	void setRareItem(bool rare) { rareItemFound = rare; }

	bool isProfitable() const {
		return goldEarned > 0;
	}

};


// DERIVED CLASS Combat Session
class CombatSession : public PlaySession {
private:
	int enemiesDefeated;
	LootInfo loot;		// composition

public:
	CombatSession()
		: PlaySession(), enemiesDefeated(0), loot() {
	}

	CombatSession(const string& loc, int duration, Difficulty diff,
		int enemies, const LootInfo& lootInfo)
		: PlaySession(loc, duration, diff),
		enemiesDefeated(enemies),
		loot(lootInfo) {
	}

	int getEnemiesDefeated() const {
		return enemiesDefeated;
	}

	void setEnemiesDefeated(int enemies) {
		enemiesDefeated = enemies;
	}

	double calculateValue() const override {
		return enemiesDefeated * 10.0;
	}

	void print() const {
		PlaySession::print();   // REQUIRED

		cout << "Enemies Defeated: " << enemiesDefeated << endl;
		cout << "Gold Earned: " << loot.getGoldEarned() << endl;
		cout << "Rare Item Found: "
			<< (loot.hasRareItem() ? "Yes" : "No") << endl;
	}

	bool operator==(const CombatSession& other) const {
		return location == other.location &&
			durationMinutes == other.durationMinutes &&
			enemiesDefeated == other.enemiesDefeated;
	}

	// Override toStream in derived class
	void toStream(std::ostream& os) const override {
		os << "Combat at " << location
			<< " | Enemies: " << enemiesDefeated;
	}
};


// DERIVED CLASS ExplorationSession
class ExplorationSession : public PlaySession {
private:
	int areasDiscovered;
	LootInfo loot;    // composition

public:
	ExplorationSession()
		: PlaySession(), areasDiscovered(0), loot() {
	}

	ExplorationSession(const string& loc, int duration,
		Difficulty diff, int areas, const LootInfo& lootInfo)
		: PlaySession(loc, duration, diff),
		areasDiscovered(areas),
		loot(lootInfo) {
	}

	int getAreasDiscovered() const {
		return areasDiscovered;
	}

	void setAreasDiscovered(int areas) {
		areasDiscovered = areas;
	}

	double calculateValue() const override {
		return areasDiscovered * 5.0;
	}

	void print() const {
		PlaySession::print();   // REQUIRED

		cout << "Areas Discovered: " << areasDiscovered << endl;
		cout << "Gold Earned: " << loot.getGoldEarned() << endl;
	}

	// Override toStream in derived class
	void toStream(std::ostream& os) const override {
		os << "Exploration at " << location
			<< " | Areas: " << areasDiscovered;
	}

};


// Neww Template Container / DELETE class SESSION MANAGER
template <typename T>
class DynamicArray {
private:
	T* items;
	int size;
	int capacity;

	void resize() {
		capacity *= 2;

		T* newArray = new T[capacity];

		for (int i = 0; i < size; i++) {
			newArray[i] = items[i];
		}

		delete[] items;
		items = newArray;
	}

public:
	DynamicArray(int initialCapacity = 2)
		: size(0), capacity(initialCapacity)
	{
		items = new T[capacity];
	}

	~DynamicArray() {
		delete[] items;
	}

	void add(T item) {
		if (this->size >= this->capacity) {
			this->resize();
		}

		this->items[this->size++] = item;
	}

	bool remove(int index) {
		if (index < 0 || index >= size)
			return false;

		for (int i = index; i < size - 1; i++) {
			items[i] = items[i + 1];
		}

		size--;
		return true;
	}

	T operator[](int index) const {
		if (index < 0 || index >= size)
			return T();
		return items[index];
	}

	DynamicArray& operator+=(T item) {
		add(item);
		return *this;
	}

	DynamicArray& operator-=(int index) {
		remove(index);
		return *this;
	}

	int getSize() const {
		return size;
	}
};

// Structs are passed by reference to avoid unnecessary copying


// Define functions
// Banner & Menu
void displayBanner();
void displayMenu();

// Character functions
void createCharacter(Character& player);
void displayCharacterSummary(const Character& player);

// Validation for string, int and double for charcter sheet
string getValidString(const string& prompt);
int getValidInt(const string& prompt, int min, int max);
double getValidDouble(const string& prompt, double min);


// Calculations & decisions

void recommendDifficulty(const Character& player, const PlaySession* sessions[], int count);
double calculateGoldPerHour(double gold, double totalHours);

// Enum-based recommendation (testable)
Difficulty recommendDifficultyByStats(int level, double avgHours);

// Transformed functions from old struct
void addSession(const PlaySession* sessions[], int& count);
void displaySessions(const PlaySession* sessions[], int count);
double calculateAverageEnemies(const PlaySession* sessions[], int count);
double calculateKillRate(const PlaySession* sessions[], int count);
double findLongestSession(const PlaySession* const sessions[], int count);

// Global operator
std::ostream& operator<<(std::ostream& os, const PlaySession& session) {
	session.toStream(os);  // polymorphic call
	return os;
}

// File output
void saveReport(const Character& player, const PlaySession* sessions[], int count);

template <typename T>
T addValues(T a, T b) {
	return a + b;
}

#ifndef _DEBUG
int main() {

	Character player;
	DynamicArray<PlaySession*> manager;
	int choice;

	displayBanner();
	createCharacter(player);
	displayCharacterSummary(player);

	do {
		displayMenu();
		choice = getValidInt("Enter choice (1-6): ", 1, 6);

		switch (choice) {

		case 1:   // Add Session
		{
			cout << "\n1. Combat Session\n";
			cout << "2. Exploration Session\n";

			int type = getValidInt("Choose session type: ", 1, 2);

			string location = getValidString("Enter location: ");
			int duration = getValidInt("Enter duration (minutes): ", 1, 600);

			cout << "Difficulty:\n1. Explorer\n2. Balanced\n3. Tactician\n";
			Difficulty diff = static_cast<Difficulty>(
				getValidInt("Choice: ", 1, 3)
				);

			int gold = getValidInt("Gold earned: ", 0, 100000);
			bool rare = getValidInt("Rare item found? (1=yes, 0=no): ", 0, 1);
			LootInfo loot(gold, rare);

			if (type == 1) {
				int enemies = getValidInt("Enemies defeated: ", 0, MAX_ENEMIES);
				manager += new CombatSession(location, duration, diff, enemies, loot);
			}
			else {
				int areas = getValidInt("Areas discovered: ", 0, 100);
				manager += new ExplorationSession(location, duration, diff, areas, loot);
			}

			cout << "Session added.\n";
			break;
		}

		case 2:   // View Sessions
		{
			if (manager.getSize() == 0) {
				cout << "No sessions recorded.\n";
			}
			else {
				cout << "\n=== Session Summary ===\n";
				for (int i = 0; i < manager.getSize(); i++) {
					cout << "Session #" << i << endl;
					manager[i]->print();  // polymorphism
					cout << "----------------------\n";
				}
			}
			break;
		}

		case 3:   // Remove Session
		{
			if (manager.getSize() == 0) {
				cout << "No sessions to remove.\n";
				break;
			}

			int index = getValidInt(
				"Enter session index to remove: ",
				0,
				manager.getSize() - 1
			);

			if (index >= 0 && index < manager.getSize()) {
				delete manager[index];   // delete object
				manager -= index;        // remove pointer
				cout << "Session removed.\n";
			}
			else {
				cout << "Invalid index.\n";
			}
			break;
		}

		case 4:   // Recommend Difficulty
		{
			if (manager.getSize() == 0) {
				cout << "No sessions available.\n";
				break;
			}

			int totalMinutes = 0;
			for (int i = 0; i < manager.getSize(); i++) {
				totalMinutes += manager[i]->getDuration();
			}

			double avgHours =
				(totalMinutes / 60.0) / manager.getSize();

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

			for (int i = 0; i < manager.getSize(); i++) {
				outFile << "Session #" << i << ":\n";
				manager[i]->print();
				outFile << "\n";
			}

			outFile.close();
			cout << "Report saved to report.txt\n";
			break;
		}

		case 6:   // Quit
			cout << "Exiting Adventure Tracker. Goodbye!\n";
			break;
		}

	} while (choice != 6);

	for (int i = 0; i < manager.getSize(); i++) {
		delete manager[i];
	}

	#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	#endif

	return 0;
}
#endif



// Banner Function
void displayBanner() {
	cout << "==============================================\n";
	cout << "     Baldur's Gate 3 - Adventure Tracker\n";
	cout << " Track your character's journey and progress\n";
	cout << "==============================================\n\n";
}

// Menu Display Function
void displayMenu() {
	cout << "\n=== Main Menu ===\n";
	cout << "1. Add Session\n";
	cout << "2. View Session Summary\n";
	cout << "3. Remove Session\n";
	cout << "4. Recommend Difficulty\n";
	cout << "5. Save Report to File\n";
	cout << "6. Quit\n\n";
}



// Create Character Sheet Function
// Collects user input, validates and stores it in a character struct passed by reference
void createCharacter(Character& player) {
	cout << "=== Character Creation ===\n\n";

	// Get character data
	player.name = getValidString("Enter your character name: ");
	player.level = getValidInt("Enter character level (1-12): ", MIN_LEVEL, MAX_LEVEL);
	player.gold = getValidDouble("Enter starting gold: ", 0.0);

	// Get difficulty
	cout << "\nSelect Difficulty:\n";
	cout << "1. Explorer\n";
	cout << "2. Balanced\n";
	cout << "3. Tactician\n";

	int choice = getValidInt("Enter choice (1-3): ", 1, 3);
	player.difficulty = static_cast<Difficulty>(choice);

	// Display confirmation
	cout << "\nCharacter created successfully!\n\n";
}


// View Character Sheet Summary Function

void displayCharacterSummary(const Character& player) {
	cout << left << setw(15) << "Name: " << player.name << endl;
	cout << left << setw(15) << "Level: " << player.level << endl;
	cout << left << setw(15) << "Gold: " << fixed << setprecision(2) << player.gold << endl;

	cout << left << setw(15) << "Difficulty: ";
	switch (player.difficulty) {
		case EXPLORER: cout << "Explorer"; break;
		case BALANCED: cout << "Balanced"; break;
		case TACTICIAN: cout << "Tactician"; break;
	}

	cout << "\n\n";
}


// Menu items

// Adds data to session array
void addSession(const PlaySession* sessions[], int& count) {
	if (count >= MAX_SESSIONS) {
		cout << "Session limit reached.\n";
		return;
	}

	cout << "\n1. Combat Session\n";
	cout << "2. Exploration Session\n";

	int type = getValidInt("Choose session type: ", 1, 2);

	string location = getValidString("Enter location: ");
	int duration = getValidInt("Enter duration (minutes): ", 1, 600);

	cout << "Difficulty:\n1. Explorer\n2. Balanced\n3. Tactician\n";
	Difficulty diff = static_cast<Difficulty>(
		getValidInt("Choice: ", 1, 3)
		);

	int gold = getValidInt("Gold earned: ", 0, 100000);
	bool rare = getValidInt("Rare item found? (1=yes, 0=no): ", 0, 1);
	LootInfo loot(gold, rare);

	if (type == 1) {
		int enemies = getValidInt("Enemies defeated: ", 0, MAX_ENEMIES);
		sessions[count++] =
			new CombatSession(location, duration, diff, enemies, loot);
	}
	else {
		int areas = getValidInt("Areas discovered: ", 0, 100);
		sessions[count++] =
			new ExplorationSession(location, duration, diff, areas, loot);
	}

	cout << "Session added.\n";
}


void displaySessions(const PlaySession* sessions[], int count) {
	if (count == 0) {
		cout << "No sessions recorded.\n";
		return;
	}

	cout << "\n=== Session Summary ===\n";

	for (int i = 0; i < count; i++) {
		sessions[i]->print();   
		cout << "-----------------\n";
	}
}



// Validation Functions

string getValidString(const string& prompt) {
	string input;

	do {
		cout << prompt;
		getline(cin, input);

		if (input.empty()) {
			cout << "Input cannot be empty. Please try again.\n";
		}
	} while (input.empty());

	return input;
}

int getValidInt(const string& prompt, int min, int max) {
	int value;

	while (true) {
		cout << prompt;
		cin >> value;

		if (cin.fail() || value < min || value > max) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid input. Please enter a value between " << min << " and " << max << ".\n";
		}
		else {
			cin.ignore(1000, '\n');
			return value;
		}
	}
}

double getValidDouble(const string& prompt, double min) {
	double value;

	while (true) {
		cout << prompt;
		cin >> value;

		if (cin.fail() || value < min) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid input. Please enter a value greater than or equal to " << min << ".\n";
		}
		else {
			cin.ignore(1000, '\n');
			return value;
		}
	}
}

// Calculate Total Hours
int calculateTotalDuration(const PlaySession* const sessions[], int count) {
	int total = 0;
	for (int i = 0; i < count; i++) {
		total += sessions[i]->getDuration();
	}
	return total;
}



// Calculate Recommended Difficulty
void recommendDifficulty(const Character& player, const PlaySession* sessions[], int count) {
	if (count == 0) {
		cout << "No sessions available.\n";
		return;
	}

	int totalMinutes = calculateTotalDuration(sessions, count);
	double avgHours = (totalMinutes / 60.0) / count;

	Difficulty rec = recommendDifficultyByStats(player.level, avgHours);

	cout << "\n=== Difficulty Recommendation ===\n";
	switch (rec) {
	case EXPLORER: cout << "Explorer\n"; break;
	case BALANCED: cout << "Balanced\n"; break;
	case TACTICIAN: cout << "Tactician\n"; break;
	}
}



// Calculate Avg Enemies killed per session
double calculateAverageEnemies(const PlaySession* sessions[], int count) {
	if (count == 0) return 0.0;

	int totalEnemies = 0;
	int combatCount = 0;

	for (int i = 0; i < count; i++) {
		const CombatSession* combat =
			dynamic_cast<const CombatSession*>(sessions[i]);

		if (combat) {
			totalEnemies += combat->getEnemiesDefeated();
			combatCount++;
		}
	}

	if (combatCount == 0) return 0.0;
	return static_cast<double>(totalEnemies) / combatCount;
}


// Calculate kill rate
double calculateKillRate(const PlaySession* sessions[], int count) {
	int totalMinutes = calculateTotalDuration(sessions, count);
	if (totalMinutes == 0) return 0.0;

	int totalEnemies = calculateTotalEnemies(sessions, count);
	return totalEnemies / (totalMinutes / 60.0);
}



// Calculate gold per hour
double calculateGoldPerHour(double gold, double totalHours) {
	if (totalHours <= 0.0) return 0.0;
	if (gold < 0.0) return 0.0;

	return gold / totalHours;
}

// Recommend difficulty by stats
Difficulty recommendDifficultyByStats(int level, double avgHours) {
	if (level < 5 && avgHours > 4.0) {
		return EXPLORER;
	}
	else if (level >= 5 && level <= 8 && avgHours >= 3.0) {
		return BALANCED;
	}
	else if (level > 8 && avgHours >= 5.0) {
		return TACTICIAN;
	}
	else {
		return BALANCED;
	}
}

// Calculate total enemies
int calculateTotalEnemies(const PlaySession* const sessions[], int count) {
	int total = 0;

	for (int i = 0; i < count; i++) {
		const CombatSession* combat =
			dynamic_cast<const CombatSession*>(sessions[i]);

		if (combat) {
			total += combat->getEnemiesDefeated();
		}
	}

	return total;
}



// Find longest session
double findLongestSession(const PlaySession* const sessions[], int count) {
	if (count == 0) return 0.0;

	int longest = sessions[0]->getDuration();

	for (int i = 1; i < count; i++) {
		if (sessions[i]->getDuration() > longest) {
			longest = sessions[i]->getDuration();
		}
	}

	return longest / 60.0; // hours
}




// File Output
void saveReport(const Character& player, const PlaySession* sessions[], int count) {
	ofstream outFile("report.txt");

	outFile << "Baldur's Gate 3 - Adventure Report\n\n";
	outFile << "Character: " << player.name << endl;
	outFile << "Level: " << player.level << endl;
	outFile << fixed << setprecision(2);
	outFile << "Gold: " << player.gold << "\n\n";

	for (int i = 0; i < count; i++) {
		outFile << "Session " << i + 1 << ":\n";
		sessions[i]->print();   // reuse polymorphism
		outFile << "\n";
	}

	outFile.close();
	cout << "Report saved to report.txt\n";
}




#ifdef _DEBUG

// ===================== UNIT TESTS =====================

// ---------- A) Abstract Base & Virtual Function ----------

TEST_CASE("Derived classes implement calculateValue polymorphically") {
	LootInfo loot(0, false);

	PlaySession* c = new CombatSession("Camp", 30, BALANCED, 5, loot);
	PlaySession* e = new ExplorationSession("Forest", 60, EXPLORER, 4, loot);

	CHECK(c->calculateValue() == 50.0);   // 5 enemies * 10
	CHECK(e->calculateValue() == 20.0);   // 4 areas * 5

	delete c;
	delete e;
}

// ---------- B) Constructor Initialization ----------

TEST_CASE("CombatSession constructor initializes correctly") {
	LootInfo loot(100, true);
	CombatSession cs("Ruins", 45, TACTICIAN, 8, loot);

	CHECK(cs.getLocation() == "Ruins");
	CHECK(cs.getDuration() == 45);
	CHECK(cs.getDifficulty() == TACTICIAN);
	CHECK(cs.getEnemiesDefeated() == 8);
}

TEST_CASE("ExplorationSession constructor initializes correctly") {
	LootInfo loot(50, false);
	ExplorationSession es("Cave", 90, BALANCED, 3, loot);

	CHECK(es.getLocation() == "Cave");
	CHECK(es.getDuration() == 90);
	CHECK(es.getDifficulty() == BALANCED);
	CHECK(es.getAreasDiscovered() == 3);
}

// ---------- F) Equality Tests -----------------

TEST_CASE("CombatSession equality operator") {
	LootInfo loot(0, false);

	CombatSession a("Camp", 30, BALANCED, 5, loot);
	CombatSession b("Camp", 30, BALANCED, 5, loot);
	CombatSession c("Forest", 20, BALANCED, 2, loot);

	CHECK(a == b);
	CHECK_FALSE(a == c);
}

// ---------- G) << Tests ------------------------

TEST_CASE("operator<< outputs correctly") {
	LootInfo loot(0, false);
	CombatSession cs("Camp", 30, BALANCED, 5, loot);

	std::ostringstream oss;
	oss << cs;

	CHECK(oss.str() == "Combat at Camp | Enemies: 5");
}

// ---------- H) [] Tests ------------------------

TEST_CASE("operator[] works with bounds checking") {
	DynamicArray<int> arr;
	arr += 10;

	CHECK(arr[0] == 10);
	CHECK(arr[5] == 0);
}

// ---------- I) Template Function Tests ----------

TEST_CASE("function template works") {
	CHECK(addValues(2, 3) == 5);
	CHECK(addValues(1.5, 2.5) == 4.0);
}

#endif
