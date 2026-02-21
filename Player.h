#pragma once

#include "Point.h"
#include "Screen.h"
#include <string>

enum class ItemType {
    NONE,
    KEY,
    TORCH,
    BOMB
};

class Player {
    static constexpr int NUM_KEYS = 6;
    Point body;
    Point prevPos;
    char keys[NUM_KEYS];
    Screen& screen;
    bool hasKey = false;
    ItemType currentItem = ItemType::NONE;
    int keysCollected = 0;
    bool reached_exit = false;
    std::string last_message = "";
    
    // Riddle & Movement Flags
    bool doorOpenedThisTurn = false; 
    bool riddleTriggered = false; 
    Point targetPos;

    // Spring Logic
    int springCyclesLeft = 0;
    int springSpeed = 0;
    int springDirX = 0;
    int springDirY = 0;
    int activeSpringForce = 0;

    bool tryHandleSwitch(const Point& nextPos, char tile);
    bool tryPushObstacle(const Point& nextPos, const Player* otherPlayer, int forceOverride = -1);
    void tryCollectItem(const Point& nextPos, char tile);
    bool tryEnterDoor(const Point& nextPos, char tile);

public:
    Player(const Point& startPoint, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen);
    void move(const Player* otherPlayer = nullptr);
    ItemType handleKeyPressed(char key);
    void setPosition(const Point& newPos);
    const Point& getPosition() const { return body; }
    
    bool hasCollectedKey() const { return hasKey; }
    bool hasTorch() const { return currentItem == ItemType::TORCH; }
    bool hasBomb() const { return currentItem == ItemType::BOMB; }
    
    void collectItem(char itemChar);
    char disposeItem();
    void reset();
    int getKeysAmount() const { return keysCollected; }
    bool reachedExit() const { return reached_exit; }
    const std::string& getLastMessage() const { return last_message; }
    void clearMessage() { last_message = ""; }

    // Riddle & Movement Methods
    bool getDidOpenDoor() const { return doorOpenedThisTurn; }
    void resetDoorFlag() { doorOpenedThisTurn = false; }
    ItemType getCurrentItem() const { return currentItem; };
    bool getRiddleTriggered() const { return riddleTriggered; }
    void resetRiddleFlag() { riddleTriggered = false; }
    Point getTargetPos() const { return targetPos; }
    void bounceBack() { body = prevPos; }
    void stopMovement() { body.setDirection(Direction::STAY); }

    // Spring Methods
    void activateSpring(int force, int dx, int dy);
    int getPushForce() const {
        if (springCyclesLeft > 0) return springSpeed;
        return 1;
    }
};
