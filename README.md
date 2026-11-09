
Slay the Maths - Mathematical Card Strategy Battle Game

A Roguelike mathematical card battle game developed using C++ and the Raylib graphics library. This project combines the classic card battle mechanics of Slay the Spire with middle-school algebraic puzzles, aiming to enhance players' motivation for mathematical calculation and strategic thinking through gamification.

Core Game Mechanics

1. Finite State Machine Driven: The system employs a rigorous State Machine architecture (Menu, Player Turn, View Pile, Math Quiz, Enemy Turn, Game Over) to ensure smooth transitions between game scenes.
2. Dynamic Deck Management: It implements a complete card game framework, including drawing logic, a maximum hand size limit of 7 cards, a discard pile, and automatic deck reshuffling accompanied by smooth card movement animations.
3. Combat and Quiz Hybrid System:
	• Playing cards allows players to accumulate potential damage and populate a pending quiz queue.
	• Ending the turn transitions the game into a mathematical algebra challenge with a 10-second time limit per question.
	• Scoring Algorithm: Answering correctly grants a damage multiplier based on response speed (ranging from approximately 1.05x to 1.5x); answering incorrectly or running out of time halves the base damage as a penalty mechanism.
4. Diverse Enemy AI: Features multiple enemy types, including Nibbit, Anthony, Bygone Effigy, and Hexagonal Ghost, each possessing distinct action intents such as single attacks, defensive shielding, stat buffs, or multi-stage ultimate abilities.

Architecture and Technology Stack

• Programming Language: C++ (C++11 or later)
• Graphics Rendering Library: Raylib (for window management, font rendering, and real-time animations)
• Design Patterns: Object-Oriented Programming (OOP), Finite State Machine (FSM)
• Resolution Adaptation: Supports dynamic window resizing using a virtual canvas technique (1280x720) to automatically apply proportional letterbox or pillarbox scaling compensation.

Project Directory Structure

• main.cpp - Core game loop, state machine flow, and main rendering logic.
• Card.h / Card.cpp - Definitions for card attributes, the card database, and movement structures.
• Character.h / Character.cpp - Attributes, intents, health points, and shielding systems for both the player and enemies.
• QuestionGenerator.h / QuestionGenerator.cpp - The backend engine responsible for generating dynamic algebraic questions and random multiple-choice options.
• UIComponents.h / UIComponents.cpp - Custom UI rendering and interaction logic for buttons and dynamic countdown timer bars.
• compiler/ - Embedded compilation environment containing pre-configured tools to build the game without manual setup.
• assets/ - Directory containing the game icon and external typography assets such as JetBrains Mono.

Installation and Execution


Running the Pre-compiled Version

1. Click the Code button on this GitHub page and select Download ZIP to download and extract the repository.
2. Ensure that the assets directory remains in the same folder level as the executable binary.
3. Double-click main.exe to launch the game directly.

Modifying and Recompiling the Source Code

This repository includes a standalone embedded compiler environment. Users do not need to install any external IDEs, software packages, or manually configure system environment variables to test code modifications.
1. Open and edit any of the source files (such as main.cpp) using a basic text editor.
2. Double-click run.bat in the root directory.
3. The automated script will call the internal compiler, link libraylib.a, and compile a clean main.exe binary instantly.
