#pragma once

#include "Bomb.h"
#include "Config.h"
#include "InputController.h"
#include "Player.h"
#include "Results.h"
#include "Riddle.h"
#include "Screen.h"
#include "Steps.h"
#include <memory>
#include <string>
#include <vector>

class GameManager {
private:
  GameMode mode = GameMode::REGULAR;
  bool isSilent = false;
  size_t iteration = 0;
  Steps currentSteps;
  Results currentResults;
  Screen screen;
  Player p1;
  Player p2;
  int currentLevel = 0;
  std::vector<std::string> levelFiles;
  int shared_score = 0; // shared between players
  int shared_lives = 3; // shared between players

  std::unique_ptr<InputController> _inputStrategy;

  static constexpr char KEYS_P1[] = "wdxase";
  static constexpr char KEYS_P2[] = "ilmjko";

  enum class MenuOption : char { START = '1', INSTRUCTIONS = '8', EXIT = '9' };

  static constexpr int ESC = 27; // ASCII value

  std::string tempMessage = "";
  int messageTimer = 0;
  static constexpr int MESSAGE_DURATION = 60;

  static constexpr Point START_POS_P1 = Point(10, 10, 0, 0, '$');
  static constexpr Point START_POS_P2 = Point(50, 10, 0, 0, '&');
  std::vector<Bomb> activeBombs;
  std::vector<Riddle> allRiddles;

  bool init();
  void runGameLoop();
  bool handlePause();
  void printInstructions();
  void handleBombs();
  bool isPlayerHit(const Point &playerPos, const Point &bombPos);
  void startLevel(int levelIndex);
  bool handleInput(bool isEndScreen);
  void updateGameLogic();
  void updateMessages();
  bool checkLevelTransition();
  void scanLevels();
  void loadRiddles();
  bool triggerRiddle();
  void printLegend();
  void addScore(int points) { shared_score += points; }
  void removeLife() {
    shared_lives--;
    if (mode != GameMode::REGULAR)
      currentResults.addEvent(iteration, Results::LIFE_LOST, "");
  }
  void checkStatUpdates();
  std::string getItemName(ItemType item);
  void handleRiddleLogic(Player &p);
  void loadDoorRequirements(const std::string &filename);
  void printWinMsg();
  void printFailMsg();
  void saveFiles();
  void validateResults();

public:
  GameManager();
  void runMenu();
  void initialize(int argc, char *argv[]);
  void setMode(GameMode m) { mode = m; }
  void setSilent(bool s) { isSilent = s; }
};
