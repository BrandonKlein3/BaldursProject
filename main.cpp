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

// One individual BG3 play session
// Sessions are stored in an array so the program can track multiple entries over time
struct Session {
	string location;
	double hoursPlayed;
	int enemiesDefeated;
};

double calculateTotalHours(const Session session[], int count);

int calculateTotalEnemies(const Session sessions[], int count);

// AdventureTracker class for unit testing and session management
class AdventureTracker {
private:
	Session sessions[MAX_SESSIONS];
	int sessionCount;

public:
	// Constructor
	AdventureTracker() {
		sessionCount = 0;
	}

	// Add a session safely (non-interactive, testable)
	bool addSession(const Session& s) {
		if (sessionCount >= MAX_SESSIONS) {
			return false;
		}

		sessions[sessionCount] = s;
		sessionCount++;
		return true;
	}

	// Getter for session count
	int getSessionCount() const {
		return sessionCount;
	}

	// Total hours using internal array
	double getTotalHours() const {
		return calculateTotalHours(sessions, sessionCount);
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

// Session fuctions
void addSession(Session sessions[], int& count);
void displaySessions(const Session sessions[], int count);

// Calculations & decisions

void recommendDifficulty(const Character& player, const Session sessions[], int count);
double calculateAverageEnemies(const Session sessions[], int count);
double calculateKillRate(const Session sessions[], int count);
double calculateGoldPerHour(double gold, double totalHours);

// Enum-based recommendation (testable)
Difficulty recommendDifficultyByStats(int level, double avgHours);

// Struct / array processing helpers

double findLongestSession(const Session sessions[], int count);

// File output
void saveReport(const Character& player, const Session sessions[], int count);


#ifndef _DEBUG
int main() {
	// creates one instance of player
	Character player;
	// creates an array of sessions controlled by max sessions
	Session sessions[MAX_SESSIONS];
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
void addSession(Session sessions[], int& count) {
	if (count >= MAX_SESSIONS) {
		cout << "Session limit reached.\n";
		return;
	}

	cout << "\n=== Add New Sessions ===\n";

	sessions[count].location = getValidString("Enter location name: ");
	sessions[count].hoursPlayed = getValidDouble("Enter hours played: ", 0.1);
	sessions[count].enemiesDefeated = getValidInt("Enter enemies defeated: ", 0, MAX_ENEMIES);

	count++;

	cout << "Session added successfully!\n";
}

void displaySessions(const Session sessions[], int count) {
	if (count == 0) {
		cout << "No sessions recorded.\n";
		return;
	}

	cout << "\n=== Session Summary ===\n";
	cout << left << setw(20) << "Location" << setw(10) << "Hours" << setw(10) << "Enemies\n";

	cout << "----------------------------------------\n";

	for (int i = 0; i < count; i++) {
		cout << left << setw(20) << sessions[i].location << setw(10) << fixed << setprecision(2) << sessions[i].hoursPlayed << setw(10) << sessions[i].enemiesDefeated << endl;
	}

	double totalHours = calculateTotalHours(sessions, count);
	cout << "\nTotal Hours Played: " << totalHours << endl;

	double longest = findLongestSession(sessions, count);
	cout << "Longest Session (hours): " << longest << endl;
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
double calculateTotalHours(const Session sessions[], int count) {
	double total = 0.0;

	for (int i = 0; i < count; i++) {
		total += sessions[i].hoursPlayed;
	}

	return total;
}

// Calculate Recommended Difficulty
void recommendDifficulty(const Character& player, const Session sessions[], int count) {
	if (count == 0) {
		cout << "No sessions available for recommendation.\n";
		return;
	}

	double avgHours = calculateTotalHours(sessions, count) / count;
	double avgEnemies = calculateAverageEnemies(sessions, count);
	double killRate = calculateKillRate(sessions, count);

	cout << "Average Enemies per Session: " << avgEnemies << endl;
	cout << "Kill Rate (enemies/hour): " << killRate << endl;


	Difficulty rec = recommendDifficultyByStats(player.level, avgHours);

	cout << "\n=== Difficulty Recommendation ===\n";

	switch (rec) {
		case EXPLORER: cout << "Recommendation: Explorer\n"; break;
		case BALANCED: cout << "Recommendation: Balanced\n"; break;
		case TACTICIAN: cout << "Recommendation: Tactician\n"; break;
	}
}

// Calculate Avg Enemies killed per session
double calculateAverageEnemies(const Session sessions[], int count) {
	if (count <= 0) return 0.0;

	int totalEnemies = 0;
	for (int i = 0; i < count; i++) {
		totalEnemies += sessions[i].enemiesDefeated;
	}

	return static_cast<double>(totalEnemies) / count;
}

// Calculate kill rate
double calculateKillRate(const Session sessions[], int count) {
	double totalHours = calculateTotalHours(sessions, count);
	if (totalHours <= 0.0) return 0.0;

	int totalEnemies = 0;
	for (int i = 0; i < count; i++) {
		totalEnemies += sessions[i].enemiesDefeated;
	}

	return totalEnemies / totalHours;
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
int calculateTotalEnemies(const Session sessions[], int count) {
	if (count <= 0) return 0;

	int total = 0;
	for (int i = 0; i < count; i++) {
		total += sessions[i].enemiesDefeated;
	}

	return total;
}

// Find longest session
double findLongestSession(const Session sessions[], int count) {
	if (count <= 0) return 0.0;

	double longest = sessions[0].hoursPlayed;
	for (int i = 1; i < count; i++) {
		if (sessions[i].hoursPlayed > longest) {
			longest = sessions[i].hoursPlayed;
		}
	}

	return longest;
}


// File Output
void saveReport(const Character& player, const Session sessions[], int count) {
	ofstream outFile("report.txt");

	outFile << "Baldur's Gate 3 - Adventure Report\n\n";
	outFile << "Character: " << player.name << endl;
	outFile << "Level: " << player.level << endl;
	outFile << fixed << setprecision(2);
	outFile << "Gold: " << player.gold << "\n\n";

	outFile << left << setw(20) << "Location" << setw(10) << "Hours" << setw(10) << "Enemies\n";

	for (int i = 0; i < count; i++) {
		outFile << left << setw(20) << sessions[i].location << setw(10) << sessions[i].hoursPlayed << setw(10) << sessions[i].enemiesDefeated << endl;
	}

	outFile.close();
	cout << "Report saved to report.txt\n";
}



#ifdef _DEBUG

// ===================== UNIT TESTS =====================

// ---------- A) Calculations (at least 4 tests) ----------

TEST_CASE("calculateTotalHours normal case") {
	Session sessions[2] = {
		{"Goblin Camp", 2.5, 10},
		{"Underdark", 3.0, 20}
	};

	double result = calculateTotalHours(sessions, 2);
	CHECK(result == doctest::Approx(5.5));
}

TEST_CASE("calculateTotalHours edge case - zero sessions") {
	Session sessions[1];
	double result = calculateTotalHours(sessions, 0);
	CHECK(result == 0.0);
}

TEST_CASE("calculateGoldPerHour normal case") {
	double result = calculateGoldPerHour(100.0, 5.0);
	CHECK(result == doctest::Approx(20.0));
}

TEST_CASE("calculateGoldPerHour guard case - divide by zero") {
	double result = calculateGoldPerHour(100.0, 0.0);
	CHECK(result == 0.0);
}


// ---------- B) Enum decision logic (at least 3 tests) ----------

TEST_CASE("recommendDifficultyByStats returns Explorer") {
	Difficulty d = recommendDifficultyByStats(3, 5.0);
	CHECK(d == EXPLORER);
}

TEST_CASE("recommendDifficultyByStats returns Balanced") {
	Difficulty d = recommendDifficultyByStats(6, 3.5);
	CHECK(d == BALANCED);
}

TEST_CASE("recommendDifficultyByStats returns Tactician") {
	Difficulty d = recommendDifficultyByStats(10, 6.0);
	CHECK(d == TACTICIAN);
}


// ---------- C) Struct / array processing (at least 3 tests) ----------

TEST_CASE("calculateTotalEnemies normal case") {
	Session sessions[2] = {
		{"Forest", 2.0, 5},
		{"Dungeon", 3.0, 15}
	};

	int total = calculateTotalEnemies(sessions, 2);
	CHECK(total == 20);
}

TEST_CASE("calculateAverageEnemies one session") {
	Session sessions[1] = {
		{"Ruins", 1.5, 12}
	};

	double avg = calculateAverageEnemies(sessions, 1);
	CHECK(avg == doctest::Approx(12.0));
}

TEST_CASE("findLongestSession edge case - zero sessions") {
	Session sessions[1];
	double longest = findLongestSession(sessions, 0);
	CHECK(longest == 0.0);
}


// ---------- D) Class methods (at least 2 tests) ----------

TEST_CASE("AdventureTracker addSession and getSessionCount") {
	AdventureTracker tracker;

	Session s = { "Shadow Cursed Lands", 4.0, 30 };
	bool added = tracker.addSession(s);

	CHECK(added == true);
	CHECK(tracker.getSessionCount() == 1);
}

TEST_CASE("AdventureTracker getTotalHours") {
	AdventureTracker tracker;

	tracker.addSession({ "Camp", 2.0, 5 });
	tracker.addSession({ "Tower", 3.5, 12 });

	double total = tracker.getTotalHours();
	CHECK(total == doctest::Approx(5.5));
}

#endif
