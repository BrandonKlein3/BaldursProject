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
#include <stdexcept>
#include <vector>
#include <algorithm>

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

// Exception Class
class ContainerException : public std::runtime_error {
public:
	ContainerException(const std::string& message)
		: std::runtime_error(message) {}
};

// Session Container replaces old template class
class SessionContainer {
private:
	std::vector<PlaySession*> items;

public:
	void add(PlaySession* item) {
		items.push_back(item);
	}

	void remove(int index) {
		if (index < 0 || index >= items.size()) {
			throw ContainerException("Invalid removal index.");
		}

		delete items[index];
		items.erase(items.begin() + index);
	}

	PlaySession* at(int index) {
		if (index < 0 || index >= items.size()) {
			throw ContainerException("Index out of bounds.");
		}
		return items.at(index);
	}

	int size() const {
		return items.size();
	}

	// -------- LINEAR SEARCH --------
	int linearSearch(const string& location) {
		for (int i = 0; i < items.size(); i++) {
			if (items[i]->getLocation() == location) {
				return i;
			}
		}
		return -1;
	}

	// -------- BUBBLE SORT --------
	void bubbleSortByDuration() {
		for (int i = 0; i < items.size() - 1; i++) {
			for (int j = 0; j < items.size() - i - 1; j++) {
				if (items[j]->getDuration() > items[j + 1]->getDuration()) {
					swap(items[j], items[j + 1]);
				}
			}
		}
	}

	// -------- BINARY SEARCH --------
	int binarySearchByDuration(int target) {
		int low = 0;
		int high = items.size() - 1;

		while (low <= high) {
			int mid = (low + high) / 2;

			int value = items[mid]->getDuration();

			if (value == target) return mid;
			else if (value < target) low = mid + 1;
			else high = mid - 1;
		}

		return -1;
	}

	// -------- DESTRUCTOR --------
	~SessionContainer() {
		for (int i = 0; i < items.size(); i++) {
			delete items[i];
		}
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
	SessionContainer manager;
	int choice;

	displayBanner();
	createCharacter(player);
	displayCharacterSummary(player);

	do {
		displayMenu();
		choice = getValidInt("Enter choice (1-9): ", 1, 9);

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
				manager.add(new CombatSession(location, duration, diff, enemies, loot));
			}
			else {
				int areas = getValidInt("Areas discovered: ", 0, 100);
				manager.add(new ExplorationSession(location, duration, diff, areas, loot));
			}

			cout << "Session added.\n";
			break;
		}

		case 2:   // View Sessions
		{
			if (manager.size() == 0) {
				cout << "No sessions recorded.\n";
			}
			else {
				cout << "\n=== Session Summary ===\n";
				for (int i = 0; i < manager.size(); i++) {
					cout << "Session #" << i << endl;
					manager.at(i)->print();  // polymorphism
					cout << "----------------------\n";
				}
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
		case 7:
		{
			string loc = getValidString("Enter location: ");
			int index = manager.linearSearch(loc);

			if (index != -1)
				cout << "Found at index: " << index << endl;
			else
				cout << "Not found.\n";

			break;
		}
		case 8:
		{
			manager.bubbleSortByDuration();
			cout << "Sorted.\n";
			break;
		}
		case 9:
		{
			int target = getValidInt("Enter duration: ", 0, 1000);

			manager.bubbleSortByDuration(); // REQUIRED
			int index = manager.binarySearchByDuration(target);

			if (index != -1)
				cout << "Found at index: " << index << endl;
			else
				cout << "Not found.\n";

			break;
		}
		}
	} while (choice != 9);

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
	cout << "6. Quit\n";
	cout << "7. Search by Location\n";
	cout << "8. Sort by Duration\n";
	cout << "9. Binary Search by Duration\n\n";
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
	CHECK(cs.getDuration() == 45);
	CHECK(cs.getDifficulty() == TACTICIAN);
	CHECK(cs.getEnemiesDefeated() == 8);
}

TEST_CASE("ExplorationSession initializes correctly") {
	LootInfo loot(50, false);
	ExplorationSession es("Cave", 90, BALANCED, 3, loot);

	CHECK(es.getLocation() == "Cave");
	CHECK(es.getDuration() == 90);
	CHECK(es.getDifficulty() == BALANCED);
	CHECK(es.getAreasDiscovered() == 3);
}

// ---------- C) Equality Operator ----------

TEST_CASE("CombatSession operator== works") {
	LootInfo loot(0, false);

	CombatSession a("Camp", 30, BALANCED, 5, loot);
	CombatSession b("Camp", 30, BALANCED, 5, loot);
	CombatSession c("Forest", 20, BALANCED, 2, loot);

	CHECK(a == b);
	CHECK_FALSE(a == c);
}

// ---------- D) Stream Operator ----------

TEST_CASE("operator<< outputs correct string") {
	LootInfo loot(0, false);
	CombatSession cs("Camp", 30, BALANCED, 5, loot);

	std::ostringstream oss;
	oss << cs;

	CHECK(oss.str() == "Combat at Camp | Enemies: 5");
}

// ---------- E) Container Add / Access ----------

TEST_CASE("SessionContainer stores and retrieves sessions") {
	SessionContainer manager;
	LootInfo loot(0, false);

	manager.add(new CombatSession("Camp", 30, BALANCED, 5, loot));

	CHECK(manager.size() == 1);
	CHECK(manager.at(0)->getLocation() == "Camp");
}

// ---------- F) Exception Handling ----------

TEST_CASE("at() throws on invalid index") {
	SessionContainer manager;

	CHECK_THROWS_AS(manager.at(0), ContainerException);
}

TEST_CASE("remove() throws on invalid index") {
	SessionContainer manager;

	CHECK_THROWS_AS(manager.remove(0), ContainerException);
}

// ---------- G) Linear Search ----------

TEST_CASE("linearSearch finds correct index") {
	SessionContainer manager;
	LootInfo loot(0, false);

	manager.add(new CombatSession("Camp", 30, BALANCED, 5, loot));
	manager.add(new ExplorationSession("Forest", 60, EXPLORER, 3, loot));

	CHECK(manager.linearSearch("Camp") == 0);
	CHECK(manager.linearSearch("Forest") == 1);
	CHECK(manager.linearSearch("Cave") == -1);
}

// ---------- H) Bubble Sort ----------

TEST_CASE("bubbleSortByDuration sorts correctly") {
	SessionContainer manager;
	LootInfo loot(0, false);

	manager.add(new CombatSession("A", 60, BALANCED, 5, loot));
	manager.add(new CombatSession("B", 30, BALANCED, 5, loot));

	manager.bubbleSortByDuration();

	CHECK(manager.at(0)->getDuration() == 30);
	CHECK(manager.at(1)->getDuration() == 60);
}

// ---------- I) Binary Search ----------

TEST_CASE("binarySearchByDuration finds correct index") {
	SessionContainer manager;
	LootInfo loot(0, false);

	manager.add(new CombatSession("A", 30, BALANCED, 5, loot));
	manager.add(new CombatSession("B", 60, BALANCED, 5, loot));

	manager.bubbleSortByDuration();

	int index = manager.binarySearchByDuration(60);

	CHECK(index != -1);
	CHECK(manager.at(index)->getDuration() == 60);
}

// ---------- J) Template Function ----------

TEST_CASE("function template works") {
	CHECK(addValues(2, 3) == 5);
	CHECK(addValues(1.5, 2.5) == 4.0);
}
#endif
