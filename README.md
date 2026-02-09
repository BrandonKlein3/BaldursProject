[![C++ doctest (Windows)](https://github.com/BrandonKlein3/BaldursProject/actions/workflows/tests.yml/badge.svg)](https://github.com/BrandonKlein3/BaldursProject/actions/workflows/tests.yml)

# Baldur’s Gate 3 – Adventure Tracker (C++)

## Overview
This project is a C++ console application that tracks a player character and multiple play sessions inspired by *Baldur’s Gate 3*.  
It was created for a college C++ programming course to demonstrate **inheritance**, **composition**, **polymorphism**, and **unit testing** using concepts covered through **Chapter 11**.

---

## Features
- Track character information (name, level, gold, difficulty)
- Record multiple play sessions
  - Combat sessions
  - Exploration sessions
- Uses:
  - **Inheritance** (`PlaySession` → `CombatSession`, `ExplorationSession`)
  - **Composition** (`LootInfo` used inside session classes)
  - **Polymorphism** via base-class pointers
- Unit testing with **doctest**
- Automated testing with **GitHub Actions**
- UML-style class diagram created using **Visual Studio Class Designer**

---

## Class Structure
- **PlaySession** (Base Class)  
  Common session data such as location, duration, and difficulty.

- **CombatSession** (Derived Class)  
  Adds enemy tracking and loot information.

- **ExplorationSession** (Derived Class)  
  Adds area discovery tracking and loot information.

- **LootInfo** (Composition Class)  
  Represents gold earned and rare item drops.

- **AdventureTracker**  
  Manages multiple sessions and provides summary statistics.

---

## Unit Tests
- Written using **doctest**
- Tests cover:
  - Constructors and getters
  - Inheritance behavior
  - Polymorphic function calls
  - Composition behavior
  - Enum-based logic
- Tests run automatically in Debug mode and through GitHub Actions

---

## How to Run
1. Open the project in **Visual Studio**
2. Run in **Debug mode** to execute unit tests
3. Run in **Release mode** to use the interactive program

---

## Requirements
- Visual Studio (with C++ workload)
- C++17 or later
- Visual Studio Class Designer (for the class diagram)

---

## Author
Me - Brandon Klein
