# 🎮 Adv-World: A 2-Player Cooperative Adventure Game

**Adv-World** is a strategic, terminal-based adventure game developed in C++. It features complex puzzle-solving mechanics, cooperative gameplay, and a robust deterministic system for recording and replaying game sessions.

---

## ✨ Key Features

### 🧩 Interactive Game Elements
* [cite_start]**➕/🚪 Switches & Doors:** Global puzzle mechanics where doors require a specific number of active switches or keys to unlock[cite: 5, 6].
* [cite_start]**🔑 Keys:** Consumable resources used to unlock specific doors throughout the levels[cite: 8, 9].
* [cite_start]**💣 Bombs:** Tactical explosives with a dual-radius logic: destroying adjacent walls while affecting soft entities within a larger radius[cite: 10, 11].
* [cite_start]**🔦 Torches:** Passive items that reveal hidden "Dark Areas" defined by dynamic map coordinates[cite: 13, 37, 38].
* [cite_start]**🧱 Connected Obstacles:** An advanced feature using a **DFS algorithm** to group touching obstacles into a single rigid body, requiring teamwork to move[cite: 15, 16, 17].
* [cite_start]**❓ Dynamic Riddles:** Challenging pop-up riddles with a robust input validation loop[cite: 18, 19].
* [cite_start]**🚀 Springs:** Launch mechanisms that propel players over obstacles based on their momentum[cite: 21, 22].

### 🛠 Technical Highlights
* [cite_start]**🏗️ Strategy Design Pattern:** The game decouples logic from input using an abstract `InputController`, allowing seamless switching between real-time keyboard play and file-based replays[cite: 47, 48, 50].
* **🔄 Deterministic Replay System:** Every session can be saved and replayed. [cite_start]By synchronizing the **Random Seed**, we ensure that random events like ghost movements and riddle selections remain identical[cite: 52, 53, 54, 55].
* [cite_start]**🗺️ Data-Driven Maps:** Custom `.screen` files support dynamic level loading, a configurable status bar (Legend), and flexible door requirements[cite: 29, 30, 31, 41].

---

## 🕹 How to Play

### Objectives & Scoring
* [cite_start]**Solving a Riddle:** +5 points[cite: 24].
* [cite_start]**Opening a Door:** +5 points[cite: 25].
* [cite_start]**Collecting a Key:** +2 points[cite: 25].
* [cite_start]**Completing a Level:** +10 points[cite: 25].

> [cite_start]⚠️ **Game Over:** The game ends if the shared lives reach 0, caused by wrong riddle answers or bomb blasts[cite: 26, 27, 28].

### Commands
Run the game from the command line:
`adv-world.exe -load|-save [-silent]`

---

## 🏗 Architecture
The project follows strict **Object-Oriented Design (OOD)** principles:
* [cite_start]**GameManager:** The central engine managing the game loop and state[cite: 50].
* [cite_start]**Input System:** Uses the Strategy Pattern to handle diverse input sources[cite: 47].
* [cite_start]**Entity Logic:** Manages specialized interactions for players, items, and environments[cite: 11, 23].

---

## 👥 Author
* [cite_start]**Ori Adani** [cite: 1]
