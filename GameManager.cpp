#include "GameManager.h"
#include "console.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

namespace fs = std::filesystem;

constexpr char GameManager::KEYS_P1[];
constexpr char GameManager::KEYS_P2[];
constexpr Point GameManager::START_POS_P1;
constexpr Point GameManager::START_POS_P2;

GameManager::GameManager()
    : p1(START_POS_P1, KEYS_P1, screen), p2(START_POS_P2, KEYS_P2, screen) {}

void GameManager::runMenu() {
  if (mode == GameMode::LOAD) {
    if (init()) {
      runGameLoop();
      validateResults();
    }
    return;
  }
  scanLevels();

  while (true) {
    clrscr();
    cout << "============================" << endl;
    cout << "            GAME            " << endl;
    cout << "============================" << endl;
    cout << " (1) Start a new game" << endl;
    cout << " (8) Instructions & Keys" << endl;
    cout << " (9) EXIT" << endl;

    char selection = 0;
    while (selection != (char)MenuOption::START &&
           selection != (char)MenuOption::INSTRUCTIONS &&
           selection != (char)MenuOption::EXIT)
      selection = (char)get_single_char();

    switch (static_cast<MenuOption>(selection)) {
    case MenuOption::START:
      if (init()) {
        runGameLoop();
        if (mode == GameMode::SAVE)
          saveFiles();
      }
      break;
    case MenuOption::INSTRUCTIONS:
      printInstructions();
      break;
    case MenuOption::EXIT:
      clrscr();
      cout << "Good Bye" << endl;
      return;
    default:
      break;
    }
  }
}

void GameManager::initialize(int argc, char *argv[]) {
  ConfigManager::parseArgs(argc, argv, mode, isSilent);
}

void GameManager::loadRiddles() {
  allRiddles.clear();
  std::ifstream file("riddles.txt");
  if (!file.is_open()) {
    cout << "Warning: riddles.txt not found. No riddles will be available."
         << endl;
    return;
  }
  std::string q, opts[Riddle::NUM_OPTS], ansStr;
  while (getline(file, q) && !q.empty()) {
    for (int i = 0; i < Riddle::NUM_OPTS; ++i) {
      getline(file, opts[i]);
    }
    getline(file, ansStr);
    char correctChar =
        ansStr.empty() ? 'A' : ansStr[0];          // default to 'A' if empty
    allRiddles.emplace_back(q, opts, correctChar); // add new riddle
  }
  file.close();
}

bool GameManager::triggerRiddle() {
  if (allRiddles.empty())
    return true;

  // Select a random riddle
  // Note: 'rand()' works correctly here because 'srand' was called once in
  // initialize()
  int randomIndex = rand() % allRiddles.size();
  Riddle &currentRiddle = allRiddles[randomIndex];

  char userChoice = 0;

  // --- Input Handling Logic ---

  if (mode == GameMode::LOAD) {
    // LOAD MODE: Get the answer recorded in the file for this iteration
    userChoice = currentSteps.getStep(iteration);

    // Visuals: Only show the riddle if NOT in silent mode
    if (!isSilent) {
      clrscr();
      currentRiddle.display();
      std::cout << "\nAnswer loaded from file: " << userChoice << std::endl;
      sleep_ms(2000); // Short delay to visualize the replay
    }
  } else {
    // REGULAR / SAVE MODE: Interaction with the real user
    bool validInput = false;
    while (!validInput) {
      clrscr();
      std::cout << "========================================" << std::endl;
      std::cout << "           ! CHALLENGE TIME !           " << std::endl;
      std::cout << "========================================\n" << std::endl;
      currentRiddle.display();
      cout << "\nEnter your answer (A, B, C, or D): ";

      userChoice = (char)get_single_char();
      userChoice = std::toupper(userChoice);

      if (userChoice >= 'A' && userChoice <= 'D') {
        validInput = true;

        // CRITICAL: In SAVE mode, we must record the user's choice to the steps
        // file so it can be replayed exactly later.
        if (mode == GameMode::SAVE) {
          currentSteps.addStep(iteration, userChoice);
        }
      } else {
        cout << "\n\nInvalid choice! Please enter A, B, C, or D." << endl;
        sleep_ms(2000);
      }
    }
  }

  // --- Logic & Outcome ---

  bool isCorrect = currentRiddle.isCorrect(userChoice);

  if (mode != GameMode::REGULAR) {
    std::string outcome = isCorrect ? "Correct" : "Wrong";
    std::string eventData = "Riddle: " + currentRiddle.getQuestion() +
                            " | Answer: " + std::string(1, userChoice) +
                            " | Result: " + outcome;

    currentResults.addEvent(iteration, Results::RIDDLE_RESULT, eventData);
  }

  if (isCorrect) {
    if (!isSilent) {
      cout << "\n\nCORRECT! Path cleared & you gained 5 Points!" << endl;
    }
    addScore(5);
    allRiddles.erase(allRiddles.begin() + randomIndex);
  } else {
    if (!isSilent) {
      cout << "\n\nWRONG! You lost a life. The path remains blocked." << endl;
    }
    removeLife();
  }

  // Pause before returning to the game (skipped in Silent mode)
  if (!isSilent) {
    cout << "\n\nPress any key to return to game..." << endl;

    if (mode != GameMode::LOAD) {
      get_single_char();
    } else {
      sleep_ms(1000); // In replay, just pause briefly
    }
  }

  return isCorrect;
}

bool GameManager::init() {
  scanLevels();
  if (levelFiles.empty()) {
    clrscr();
    cout << "Error: No .screen files found in the current directory." << endl;
    cout << "Please ensure .screen files are in: " << fs::current_path()
         << endl;
    cout << "\nPress any key to return to menu...";
    get_single_char();
    return false; // fail to init
  }
  
  // -- Reset Game State --
  iteration = 0;
  shared_lives = 3;
  shared_score = 0;
  currentLevel = 0;
  currentResults.events.clear(); // Clear previous results
  
  long seed;
  
  if (mode == GameMode::LOAD) {
    currentSteps = Steps::loadSteps("adv-world.steps");
    seed = currentSteps.getRandomSeed();
    _inputStrategy = std::make_unique<FileStepsInput>(currentSteps);
  } else {
    // SAVE or REGULAR
    currentSteps = Steps(); // Start with empty steps
    seed = static_cast<long>(time(NULL));
    currentSteps.setRandomSeed(seed);
    _inputStrategy = std::make_unique<KeyboardInput>();
  }
  
  srand(static_cast<unsigned int>(seed));
  loadRiddles();

  startLevel(0);
  return true;
}

void GameManager::runGameLoop() {
  if (!isSilent)
    screen.draw(p1, p2);

  while (true) {
    if (currentLevel >= levelFiles.size()) {
      clrscr();
      printWinMsg();

      while (true) {
        if (check_kbhit()) {
          char c = (char)get_single_char();
          if (c == ESC)
            return;
        }
      }
    }
    gotoxy(0, 0);
    if (!isSilent)
      cout.flush();

    if (!handleInput(false)) {
      return;
    }

    updateGameLogic();

    if (shared_lives <= 0) {
      clrscr();
      printFailMsg();
      if (mode != GameMode::LOAD && !isSilent)
        get_single_char();
      return;
    }

    if (checkLevelTransition()) {
      iteration++;
      continue;
    }
    if (!isSilent) {
      screen.draw(p1, p2);
      printLegend();
      cout.flush();
      sleep_ms(50);
    }
    iteration++;
  }
}

bool GameManager::handlePause() {
  clrscr();
  gotoxy(34, 10);
  std::cout << "Game Paused";
  gotoxy(34, 11);
  std::cout << "ESC - Resume";
  gotoxy(32, 12);
  std::cout << "H - Back to Menu";

  while (true) {
    if (check_kbhit()) {
      // casting
      char c = (char)get_single_char();
      c = std::tolower(c);

      if (c == ESC) {
        return true;
      }

      if (c == 'h') {
        return false;
      }
    }
    sleep_ms(50);
  }
}

void GameManager::printInstructions() {
  clrscr();
  cout << "INSTRUCTIONS:" << endl;
  cout << "=============" << endl;
  cout << "Player 1 ($) Keys:" << endl;
  cout << "  Move: w/a/d/x" << endl;
  cout << "  Stay: s" << endl;
  cout << "  Dispose/Use Item: e" << endl;
  cout << endl;
  cout << "Player 2 (&) Keys:" << endl;
  cout << "  Move: i/j/l/m" << endl;
  cout << "  Stay: k" << endl;
  cout << "  Dispose/Use Item: o" << endl;
  cout << endl;
  cout << "Game Elements:" << endl;
  cout << "  @ : Bomb     (Explodes 5 cycles after disposal)" << endl;
  cout << "  ! : Torch    (Lights up dark areas)" << endl;
  cout << "  # : Spring   (Bounces you forward)" << endl;
  cout << "  * : Obstacle (Pushable block)" << endl;
  cout << "  ? : Riddle   (Answer correctly to pass)" << endl;
  cout << "  + : Switch   (Open/Close doors)" << endl;
  cout << endl;
  cout << "Avoid walls and have fun!" << endl;
  cout << endl;
  cout << "Press any key to go back to menu...";

  (void)get_single_char();
}

void GameManager::handleBombs() {
  for (int i = activeBombs.size() - 1; i >= 0; --i) {
    bool explodeNow = activeBombs[i].tick();
    if (explodeNow) {
      activeBombs[i].explode(screen);
      Point bombPos = activeBombs[i].getposition();
      if (isPlayerHit(p1.getPosition(), bombPos)) {
        removeLife();
        p1.reset();
        p1.setPosition(screen.getStartP1());
      }
      if (isPlayerHit(p2.getPosition(), bombPos)) {
        removeLife();
        p2.reset();
        p2.setPosition(screen.getStartP2());
      }
      activeBombs.erase(activeBombs.begin() + i);
    }
  }
}

bool GameManager::isPlayerHit(const Point &playerPos, const Point &bombPos) {
  int dx = std::abs(playerPos.getX() - bombPos.getX());
  int dy = std::abs(playerPos.getY() - bombPos.getY());

  int dist = max(dx, dy);

  return dist <= 3;
}

void GameManager::startLevel(int levelIndex) {
  if (levelIndex < 0 || levelIndex >= levelFiles.size())
    return;
  currentLevel = levelIndex;
  if (!screen.loadMapFromFile(levelFiles[levelIndex]))
    return;
  loadDoorRequirements(levelFiles[levelIndex]);
  p1.reset();
  p2.reset();
  p1.setPosition(screen.getStartP1());
  p2.setPosition(screen.getStartP2());
  activeBombs.clear();
}

bool GameManager::handleInput(bool isEndScreen) {
  char action = _inputStrategy->getAction(iteration);

  if (action != 0) {
    char c = (action == ESC) ? action : std::tolower(action);

    if (mode == GameMode::SAVE && !isEndScreen) {
      currentSteps.addStep(iteration, c);
    }

    if (isEndScreen) {
      if (c == ESC)
        return false; // ESC to go back to menu, ignore other keys
      return true;
    }
    c = std::tolower(c);
    if (c == ESC) {
      if (mode == GameMode::LOAD)
        return false;
      if (handlePause() == false)
        return false;

      screen.resetDrawingState();
      screen.draw(p1, p2);
    }

    else {
      if (p1.handleKeyPressed(c) == ItemType::BOMB)
        activeBombs.push_back(Bomb(p1.getPosition()));
      if (p2.handleKeyPressed(c) == ItemType::BOMB)
        activeBombs.push_back(Bomb(p2.getPosition()));
    }
  }
  return true; // continue game
}

void GameManager::updateGameLogic() {
  p1.move(&p2);
  p2.move(&p1);
  checkStatUpdates();
  handleRiddleLogic(p1);
  handleRiddleLogic(p2);
  handleBombs();
  updateMessages();
}

void GameManager::updateMessages() {
  if (!p1.getLastMessage().empty()) {
    tempMessage = p1.getLastMessage();
    messageTimer = MESSAGE_DURATION;
    p1.clearMessage();
  } else if (!p2.getLastMessage().empty()) {
    tempMessage = p2.getLastMessage();
    messageTimer = MESSAGE_DURATION;
    p2.clearMessage();
  }

  if (messageTimer > 0) {
    messageTimer--;
    if (messageTimer == 0) {
      tempMessage = "";
    }
  }
}

bool GameManager::checkLevelTransition() {
  if (p1.reachedExit() && p2.reachedExit()) {
    if (mode != GameMode::REGULAR) {
      std::string nextLevelName = levelFiles[currentLevel + 1];
      if (nextLevelName.size() > 2 && nextLevelName.substr(0, 2) == ".\\")
        nextLevelName = nextLevelName.substr(2);
      currentResults.addEvent(iteration, Results::LEVEL_TRANSITION,
                              nextLevelName);
    }
    addScore(10);
    if (currentLevel < levelFiles.size() - 1) {
      startLevel(currentLevel + 1);
      tempMessage = "";
      messageTimer = 0;
      screen.draw(p1, p2);
      return true;
    } else {
      currentLevel++;
      return true;
    }
  }
  return false;
}

// function created by Gemini
void GameManager::scanLevels() {
  levelFiles.clear();
  for (const auto &entry : fs::directory_iterator(".")) {
    if (entry.path().extension() == ".screen") {
      levelFiles.push_back(entry.path().string());
    }
  }
  std::sort(levelFiles.begin(), levelFiles.end());
}

void GameManager::printLegend() {
  if (!screen.isLegendVisible())
    return;

  gotoxy(0, 0);

  if (messageTimer > 0)
    cout << tempMessage << "                        " << endl;
  else {
    Point loc = screen.getLegendLocation();
    // row 1 : lives & points
    gotoxy(loc.getX(), loc.getY());
    cout << "LIVES: " << shared_lives << " | SCORE: " << shared_score
         << "                                                     ";
    // row 2 : Player 1
    gotoxy(loc.getX(), loc.getY() + 1);
    cout << "P1($):" << getItemName(p1.getCurrentItem());
    cout << "|Keys: " << p1.getKeysAmount() << "    ";
    // row 3 : Player 2
    gotoxy(loc.getX(), loc.getY() + 2);
    cout << "P2(&):" << getItemName(p2.getCurrentItem());
    cout << "|Keys: " << p2.getKeysAmount() << "    ";
  }
}

void GameManager::checkStatUpdates() {
  static int lastTotalKeys = 0;
  int currentTotalKeys = p1.getKeysAmount() + p2.getKeysAmount();
  if (currentTotalKeys > lastTotalKeys) {
    addScore(2);
    lastTotalKeys = currentTotalKeys;
  }
  if (p1.getDidOpenDoor() || p2.getDidOpenDoor()) {
    addScore(5);
    p1.resetDoorFlag();
    p2.resetDoorFlag();
  }
}

std::string GameManager::getItemName(ItemType item) {
  switch (item) {
  case ItemType::TORCH:
    return "TORCH";
  case ItemType::BOMB:
    return "BOMB";
  default:
    return "EMPTY";
  }
}

void GameManager::handleRiddleLogic(Player &p) {
  if (p.getRiddleTriggered()) {
    Point riddlePos = p.getTargetPos();
    bool success = triggerRiddle();
    if (success)
      screen.setChar(riddlePos, Screen::SPACE);
    else {
      p.bounceBack();
      p.stopMovement();
    }
    p.resetRiddleFlag();
    clrscr();
    screen.resetDrawingState();
  }
}

void GameManager::loadDoorRequirements(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) { // file not found
    cout << "Warning: map file " << filename
         << " not found. No door requirements loaded." << endl;
    return;
  }

  screen.clearDoorConfig();
  std::string line;
  bool foundConfigSection = false;

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    // Check for the door configuration section
    if (line.find("# DOOR CONFIGURATION") != std::string::npos) {
      foundConfigSection = true;
      continue;
    }

    if (foundConfigSection && std::isdigit(line[0])) {
      std::stringstream ss(line);
      int doorIdx, keysReq, switchesReq;
      if (ss >> doorIdx >> keysReq >> switchesReq) {
        screen.setDoorConfig(doorIdx, keysReq, switchesReq);
      }
    }
  }
  file.close();
}

void GameManager::printWinMsg() {
  if (mode != GameMode::REGULAR)
    currentResults.addEvent(iteration, Results::GAME_FINISHED,
                            "Final Score: " + std::to_string(shared_score));
  int centerX = 15;
  int centerY = 8;

  gotoxy(centerX, centerY);
  cout << "===================================================";
  gotoxy(centerX, centerY + 1);
  cout << "                                                   ";
  gotoxy(centerX, centerY + 2);
  cout << "  CONGRATULATIONS! YOU HAVE FINISHED ALL LEVELS!   ";
  gotoxy(centerX, centerY + 3);
  cout << "                                                   ";
  gotoxy(centerX, centerY + 4);
  cout << "===================================================";

  gotoxy(centerX + 15, centerY + 6);
  cout << "Your final score: " << shared_score;

  gotoxy(centerX + 10, centerY + 8);
  cout << "Press ESC to return to main menu...";
}

void GameManager::printFailMsg() {
  int centerX = 25;
  int centerY = 10;

  gotoxy(centerX, centerY);
  cout << "===========================";
  gotoxy(centerX, centerY + 1);
  cout << "      GAME OVER!           ";
  gotoxy(centerX, centerY + 2);
  cout << "  You ran out of lives.    ";
  gotoxy(centerX, centerY + 3);
  cout << "===========================";

  gotoxy(centerX - 2, centerY + 5);
  cout << "Press any key to return to menu...";
}

void GameManager::saveFiles() {
  if (mode == GameMode::SAVE) {
    currentSteps.saveSteps("adv-world.steps");
    currentResults.saveResults("adv-world.result");

    int msgX = 28;
    int msgY = 14;

    gotoxy(msgX, msgY);
    std::cout << "Game saved to files successfully." << std::endl;
    sleep_ms(2000);
    gotoxy(msgX, msgY);
    std::cout << "                                  ";
  }
}

void GameManager::validateResults() {
  Results expectedResults = Results::loadResults("adv-world.result");

  if (currentResults.isEqual(expectedResults)) {
    if (!isSilent)
      clrscr();
    std::cout << "Test Passed" << std::endl;
  } else {
    if (!isSilent)
      clrscr();
    std::cout << "Test Failed" << std::endl;
  }
  if (!isSilent) {
    std::cout << "Press any key to exit..." << std::endl;
    get_single_char();
  }
}
