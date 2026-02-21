# 🎮 Adv-World: A 2-Player Cooperative Adventure Game

**Adv-World** is a strategic, terminal-based adventure game developed in C++. It features complex puzzle-solving mechanics, cooperative gameplay, and a robust deterministic system for recording and replaying game sessions.

---

## ✨ Key Features

### 🧩 Interactive Game Elements
* **➕/🚪 Switches & Doors:** Global puzzle mechanics where doors require a specific number of active switches or keys to unlock.
* **🔑 Keys:** Consumable resources used to unlock specific doors throughout the levels.
* **💣 Bombs:** Tactical explosives with a dual-radius logic: destroying adjacent walls (radius 1) while affecting soft entities within a larger radius (radius 3).
* **🔦 Torches:** Passive items that reveal hidden "Dark Areas" defined by dynamic map coordinates.
* **🧱 Connected Obstacles:** An advanced feature using a **DFS algorithm** to group touching obstacles into a single rigid body, requiring teamwork to move.
* **❓ Dynamic Riddles:** Challenging pop-up riddles with a robust input validation loop.
* **🚀 Springs:** Launch mechanisms that propel players over obstacles based on their momentum.

### 🛠 Technical Highlights
* **🏗️ Strategy Design Pattern:** The game decouples logic from input using an abstract `InputController`, allowing seamless switching between real-time keyboard play and file-based replays.
* **🔄 Deterministic Replay System:** Every session can be saved and replayed. By synchronizing the **Random Seed**, we ensure that random events like ghost movements and riddle selections remain identical.
* **🗺️ Data-Driven Maps:** Custom `.screen` files support dynamic level loading, a configurable status bar (Legend), and flexible door requirements.

---

## 🕹 How to Play

### Objectives & Scoring
* **Solving a Riddle:** +5 points.
* **Opening a Door:** +5 points.
* **Collecting a Key:** +2 points.
* **Completing a Level:** +10 points.

> ⚠️ **Game Over:** The game ends if the shared lives reach 0, caused by wrong riddle answers or bomb blasts.

### Commands
Run the game from the command line:
`adv-world.exe -load|-save [-silent]`

---

## 🏗 Architecture
The project follows strict **Object-Oriented Design (OOD)** principles:
* **GameManager:** The central engine managing the game loop and state.
* **Input System:** Uses the Strategy Pattern to handle diverse input sources.
* **Entity Logic:** Manages specialized interactions for players, items, and environments.

---

## 👥 Author
* **Ori Adani**
