#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

// Goal: Track player character and multiple play sessions while practicing C++ fundamentals
// Enum represents the possible difficuly recommendation that will be used later in switch
// statements and conditional logic
enum Difficulty {
	EXPLORER,
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

// Constants
// Avoids magic numbers and improves readability.
const int MAX_SESSIONS = 10;
const int MIN_LEVEL = 1;
const int MAX_LEVEL = 12;
const int MAX_ENEMIES = 1000;

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
double calculateTotalHours(const Session session[], int count);
void recommendDifficulty(const Character& player, const Session sessions[], int count);

// File output
void saveReport(const Character& player, const Session sessions[], int count);



int main() {
	Character player;
	Session sessions[MAX_SESSIONS];
	int sessionCount = 0;
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

	cout << "\n=== Difficulty Recommendation ===\n";

	if (player.level < 5 && avgHours > 4) {
		cout << "Recommendation: Explorer\n";
	}
	else if (player.level >= 5 && player.level <= 8 && avgHours >= 3) {
		cout << "Recommendation: Balanced\n";
	}
	else if (player.level > 8 && avgHours >= 5) {
		cout << "Recommendation: Tactician\n";
	}
	else {
		cout << "Recommendation: Balanced\n";
	}
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