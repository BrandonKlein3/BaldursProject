#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN //only works while in debug
#include "doctest.h"
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

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

	virtual void print() const {
		cout << "Location: " << location << endl;
		cout << "Duration (minutes): " << durationMinutes << endl;
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

	void print() const {
		PlaySession::print();   // REQUIRED

		cout << "Enemies Defeated: " << enemiesDefeated << endl;
		cout << "Gold Earned: " << loot.getGoldEarned() << endl;
		cout << "Rare Item Found: "
			<< (loot.hasRareItem() ? "Yes" : "No") << endl;
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

	void print() const {
		PlaySession::print();   // REQUIRED

		cout << "Areas Discovered: " << areasDiscovered << endl;
		cout << "Gold Earned: " << loot.getGoldEarned() << endl;
	}

};

// AdventureTracker class for unit testing and session management
class AdventureTracker {
private:
	const PlaySession* sessions[MAX_SESSIONS];
	int sessionCount;

public:
	// Constructor
	AdventureTracker() {
		sessionCount = 0;
	}

	// Add a session safely (non-interactive, testable)
	bool addSession(const PlaySession* s) {
		if (sessionCount >= MAX_SESSIONS) return false;
		sessions[sessionCount++] = s;
		return true;
	}


	// Getter for session count
	int getSessionCount() const {
		return sessionCount;
	}

	// Total hours using internal array
	double getTotalHours() const {
		return calculateTotalDuration(sessions, sessionCount) / 60.0;
	}


	// Total enemies using intenal array
	int getTotalEnemies() const {
		return calculateTotalEnemies(sessions, sessionCount);
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


// File output
void saveReport(const Character& player, const PlaySession* sessions[], int count);


#ifndef _DEBUG
int main() {
	// creates one instance of player
	Character player;
	// creates an array of sessions controlled by max sessions
	const PlaySession* sessions[MAX_SESSIONS];
	// tracks how many sessions
	int sessionCount = 0;
	// variable to store users choice
	int choice;

	displayBanner();
	createCharacter(player);
	displayCharacterSummary(player);

	
	// Main menu loop
	do {
		displayMenu();
		choice = getValidInt("Enter choice (1-5): ", 1, 5);

		switch (choice) {
			case 1:
				addSession(sessions, sessionCount);
				break;
			case 2:
				displaySessions(sessions, sessionCount);
				break;
			case 3:
				recommendDifficulty(player, sessions, sessionCount);
				break;
			case 4:
				saveReport(player, sessions, sessionCount);
				break;
			case 5:
				cout << "Exiting Adventure Tracker. Goodbye!\n";
				break;
		}
	} while (choice != 5);

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
	cout << "3. Recommend Difficulty\n";
	cout << "4. Save Report to File\n";
	cout << "5. Quit\n\n";
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

// ---------- A) Constructors & Getters ----------

TEST_CASE("PlaySession constructor initializes base fields") {
	PlaySession ps("Goblin Camp", 45, BALANCED);

	CHECK(ps.getLocation() == "Goblin Camp");
	CHECK(ps.getDuration() == 45);
	CHECK(ps.getDifficulty() == BALANCED);
}

TEST_CASE("LootInfo constructor and getters work") {
	LootInfo loot(100, true);

	CHECK(loot.getGoldEarned() == 100);
	CHECK(loot.hasRareItem() == true);
}

TEST_CASE("LootInfo isProfitable helper method") {
	LootInfo loot1(0, false);
	LootInfo loot2(25, false);

	CHECK(loot1.isProfitable() == false);
	CHECK(loot2.isProfitable() == true);
}


// ---------- B) Derived Classes & Inheritance ----------

TEST_CASE("CombatSession initializes base and derived data") {
	LootInfo loot(50, true);
	CombatSession cs("Ruins", 30, TACTICIAN, 8, loot);

	CHECK(cs.getLocation() == "Ruins");
	CHECK(cs.getDuration() == 30);
	CHECK(cs.getDifficulty() == TACTICIAN);
	CHECK(cs.getEnemiesDefeated() == 8);
}

TEST_CASE("ExplorationSession initializes base and derived data") {
	LootInfo loot(20, false);
	ExplorationSession es("Forest", 60, EXPLORER, 3, loot);

	CHECK(es.getLocation() == "Forest");
	CHECK(es.getDuration() == 60);
	CHECK(es.getDifficulty() == EXPLORER);
	CHECK(es.getAreasDiscovered() == 3);
}


// ---------- C) Polymorphism & Array Processing ----------

TEST_CASE("calculateTotalEnemies counts only CombatSession objects") {
	LootInfo loot(10, false);

	PlaySession* sessions[3];
	sessions[0] = new CombatSession("Camp", 30, BALANCED, 5, loot);
	sessions[1] = new ExplorationSession("Cave", 40, BALANCED, 2, loot);
	sessions[2] = new CombatSession("Tower", 50, TACTICIAN, 7, loot);

	int total = calculateTotalEnemies(sessions, 3);
	CHECK(total == 12);

	// cleanup
	for (int i = 0; i < 3; i++) delete sessions[i];
}

TEST_CASE("findLongestSession returns longest duration") {
	LootInfo loot(0, false);

	PlaySession* sessions[2];
	sessions[0] = new CombatSession("Camp", 20, EXPLORER, 3, loot);
	sessions[1] = new ExplorationSession("Forest", 90, BALANCED, 5, loot);

	double longest = findLongestSession(sessions, 2);
	CHECK(longest == 90);

	for (int i = 0; i < 2; i++) delete sessions[i];
}


// ---------- D) Enum Decision Logic ----------

TEST_CASE("recommendDifficultyByStats logic works") {
	CHECK(recommendDifficultyByStats(3, 5.0) == EXPLORER);
	CHECK(recommendDifficultyByStats(6, 3.5) == BALANCED);
	CHECK(recommendDifficultyByStats(10, 6.0) == TACTICIAN);
}


// ---------- E) Class Method Tests ----------

TEST_CASE("AdventureTracker addSession and count") {
	AdventureTracker tracker;

	PlaySession* s = new CombatSession(
		"Camp", 30, BALANCED, 4, LootInfo(10, false)
	);

	CHECK(tracker.addSession(s) == true);
	CHECK(tracker.getSessionCount() == 1);

	delete s;
}

#endif
