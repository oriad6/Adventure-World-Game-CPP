Names & IDs:
1. Ben Pitkovsky - 322431461
2. Ori Adani - 322354366

*** IMPORTANT NOTE REGARDING SUBMISSION TIME ***

We apologize for the slight delay in submission (less than an hour). 
The delay was caused by a specific technical bug in the file validation 
mechanism (Test Validation) during 'load' mode. 

While the game logic itself was fully functional, we encountered a 
synchronization issue between the recorded steps and the expected results 
which triggered a "Test Failed" output. We dedicated the last few hours 
to debugging and resolving this issue to ensure the project is submitted 
in a state that fully meets the requirements and passes the automated 
tests properly.


Game Elements & Logic

1. Switches & Doors (+ / D):
   - Logic: Switches are global. A door requires a specific number of ACTIVE switches on the entire map to open.
   - Example: If a door requires 2 switches, the players must find and toggle ON any 2 switches in the level.
   - Doors may also require Keys AND Switches simultaneously.

2. Keys (K):
   - Logic: Keys are consumable resources.
   - If a door requires 2 keys, opening it will remove 2 keys from the player's inventory.

3. Bombs (@):
   - Logic: Bombs have a dual blast radius:
     * Walls ('W'): Destroyed only if they are adjacent (Distance <= 1).
     * Soft Entities (Players / Items): Destroyed within a larger radius (Distance <= 3).
   - Timer: Bombs tick for a set duration before exploding.

4. Torches (!):
   - Logic: Passive inventory item.
   - When a player holding a torch enters a "Dark Area", the area becomes visible. Leaving the area (or not having a torch) keeps it hidden.

5. Obstacles (*):
   - Algorithm: We implemented a Connected Components (DFS) algorithm (Advanced Feature).
   - Behavior: Obstacles that touch each other form a single rigid body.
   - To move a large obstacle (mass > 1), players must cooperate (Push together) to combine their force.

6. Riddle (?):
   -Trigger: Triggered when a player attempts to move into a square containing a ?.
   -Input Validation: The riddle screen includes a robust loop that only accepts valid inputs (A, B, C, or D), ignoring other keys without penalty.
   -Data Driven: Riddles are selected from a dynamic pool, ensuring variety in challenges.

7. Springs (#):
   - Logic: A mechanism that launches the player over walls or gaps.
   - Behavior: The jump distance/force depends on the "run-up" (momentum) or the compression against a wall.

8. Player's dispose Logic:
    - Priority: First disposes items and then keys when a player holds both.

9. Score & lives:
   -Scoring (global shared score): 
             Solving a Riddle: +5 points.
             Opening a Door: +5 points.
             Collecting a key: +2 points.
             Completing a level: +10 points.
   -Lives:
            Wrong Riddle Answer: Loss of 1 life.
            Bomb Blast: Loss of 1 life and respawn at level start.
            Game Over: Triggered when lives reach 0, leading to a centered "Game Over" screen.

--- File-Based Map & Logic ---
1. Map Loading (.screen):
   - Maps are loaded dynamically from .screen files using the `loadMapFromFile` function.
   - The game supports multiple levels; finishing one loads the next alphabetically.

2. Dynamic Legend (L):
   - The 'L' character in the map file marks the top-left corner of the Status Bar (Lives, Score, Inventory).
   - Logic: The game automatically "blocks" the 3 rows occupied by the Legend, creating an invisible wall so players cannot walk over the text.
   - Safety: If 'L' is placed too far right (e.g., col > 60), the game automatically shifts it left to prevent text wrapping.

3. How to Edit:
   * LEGEND ('L'): 
     Place the letter 'L' anywhere you want the Status Bar to appear.
      - Note: The game reserves 3 rows downwards from this point.
      - Important: If you place 'L' at the very bottom (without 3 empty lines below), the Legend will NOT be displayed.

   * DARK AREA ('D' / 'd'):
     To create a hidden area, you must place exactly TWO 'D' characters in the map.
     - Logic: These two points define the Top-Left and Bottom-Right corners of the dark rectangle.
     - Note: Inside the map file, use 'D' for walls inside darkness, and 'd' for empty space inside darkness (optional markers).

   * PLAYERS:
     Use '$' for Player 1 start position.
     Use '&' for Player 2 start position.

   * DOOR CONFIGURATION (Bottom of .screen file):
     The requirements for opening specific doors are defined at the very end of the file, 
     after the map layout.
     
     Format: [Door ID] [Keys Required] [Switches Required]
     
     Example:
     1 1 0   -> Door '1' requires 1 Key and 0 Switches.
     5 2 1   -> Door '5' requires 2 Keys and 1 active Switch.

     The [Door ID] (e.g., '5') represents the **Lock Configuration ID**, NOT the destination level.
     - Changing the ID changes *rules* to open the door (keys/switches needed).
     - It does NOT change where the door leads. The game progression is linear 
       (Level 1 -> Level 2 -> Level 3) regardless of which door is used to exit.

--- Architecture & Design (OOD) ---

1. Input System (Strategy Pattern):
   - Goal: Decouple the game logic from the input source to support both interactive play and recording/replay.
   - Abstract Strategy: `InputController` defines `getAction(timer)`.
   - Concrete Strategies:
     * `KeyboardInput`: Reads real-time keys using `_kbhit()`/`_getch()`.
     * `FileStepsInput`: Reads recorded steps from the `Steps` object based on the iteration timer.
   - Logic: The `GameManager` holds a pointer to the abstract strategy. It does not know if the input comes from a human or a file, ensuring the exact same code runs in both modes.

2. Deterministic Replay (Recording):
   - Logic: To ensure the "Replay" mode behaves exactly like the original run, we implemented a Random Seed mechanism.
   - Behavior: When saving, the initial random seed is written to the `.steps` file. When loading, this seed is read and applied to `srand()`.
   - Result: Random events (like Riddle selection or Ghost movement) occur in the exact same sequence during playback.
