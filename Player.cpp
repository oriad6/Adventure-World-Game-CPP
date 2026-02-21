#include "Player.h"
#include <cstring>
#include <cmath>

bool Player::tryHandleSwitch(const Point& nextPos, char tile) {
    if (tile == Screen::SWITCH_ON || tile == Screen::SWITCH_OFF) {
        screen.handleSwitch(nextPos);
        body = nextPos;
        return true;
    }
    return false;
}

bool Player::tryPushObstacle(const Point& nextPos, const Player* otherPlayer, int forceOverride) {
    int dx = body.getDiffX();
    int dy = body.getDiffY();

    if (nextPos.getX() > body.getX()) dx = 1;
    else if (nextPos.getX() < body.getX()) dx = -1;
    else dx = 0;

    if (nextPos.getY() > body.getY()) dy = 1;
    else if (nextPos.getY() < body.getY()) dy = -1;
    else dy = 0;

    if (otherPlayer != nullptr) {
        int obstacleDestX = nextPos.getX() + dx;
        int obstacleDestY = nextPos.getY() + dy;
        const Point& otherPos = otherPlayer->getPosition();
        if (otherPos.getX() == obstacleDestX && otherPos.getY() == obstacleDestY)
            return false;
    }
    int currForce = 1;
    if (forceOverride != -1)
        currForce = forceOverride;

    if (otherPlayer != nullptr) {
        const Point& otherPos = otherPlayer->getPosition();

        bool sameDirection = (dx == otherPos.getDiffX()) && (dy == otherPos.getDiffY());
        int dist = std::abs(body.getX() - otherPos.getX()) + std::abs(body.getY() - otherPos.getY());

        if (dist <= 1 && sameDirection)
            currForce += otherPlayer->getPushForce();
    }
    return screen.moveObstacle(nextPos, dx, dy, currForce);
}

void Player::tryCollectItem(const Point& nextPos, char tile) {

    if (tile == Screen::KEY) {
        collectItem(tile);
        screen.setChar(nextPos, Screen::SPACE);
    }
    else if (tile == Screen::TORCH || tile == Screen::BOMB) {
        if (currentItem == ItemType::NONE) {
            collectItem(tile);
            screen.setChar(nextPos, Screen::SPACE);
        }
        else {
            // can't pick up another item
        }
    }
}

bool Player::tryEnterDoor(const Point& nextPos, char tile) {
    if (!isdigit(tile)) return false;
    std::string msg;
    bool isOpen = screen.handleDoor(tile, keysCollected, msg);

    if (isOpen) {
        reached_exit = true;
        body = nextPos;
        doorOpenedThisTurn = true;
        return true;
    }
    else {
        last_message = msg;
        doorOpenedThisTurn = false;
        return false;
    }
}


Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen) :
    body(point), prevPos(point), screen(theScreen)
{
    memcpy(keys, the_keys, NUM_KEYS * sizeof(keys[0]));
}

void Player::move(const Player* otherPlayer) {
    // Reset per-turn flags
    doorOpenedThisTurn = false;
    riddleTriggered = false;
    prevPos = body;

    // --- Spring Flight Logic ---
    if (springCyclesLeft > 0) {
        springCyclesLeft--;

        for (int i = 0; i < springSpeed; ++i) {
            Point flyNext = body;
            if (springDirX == 0 && springDirY == -1) flyNext.setDirection(Direction::UP);
            else if (springDirX == 0 && springDirY == 1)  flyNext.setDirection(Direction::DOWN);
            else if (springDirX == -1 && springDirY == 0) flyNext.setDirection(Direction::LEFT);
            else if (springDirX == 1 && springDirY == 0)  flyNext.setDirection(Direction::RIGHT);

            flyNext.move();

            char tile = screen.getCharAt(flyNext);

            if (tile == Screen::WALL) {
                springCyclesLeft = 0;
                break;
            }

            if (tile == Screen::OBSTACLE) {
                if (tryPushObstacle(flyNext, nullptr, springSpeed)) {
                    body = flyNext;
                    if (springCyclesLeft > 0)
                        springCyclesLeft /= 2;
                    continue;
                }
                else {
                    springCyclesLeft = 0;
                    break;
                }
            }

            if (otherPlayer != nullptr && !otherPlayer->reachedExit()) {
                Point otherPos = otherPlayer->getPosition();
                if (flyNext.getX() == otherPos.getX() && flyNext.getY() == otherPos.getY()) {
                    springCyclesLeft = 0;
                    Player* victim = const_cast<Player*>(otherPlayer);
                    victim->activateSpring(springSpeed, springDirX, springDirY);
                    body.setDirection(Direction::STAY); 
                    break;
                }
            }

            if (isdigit(tile)) {
                springCyclesLeft = 0;
                break;
            }

            tryCollectItem(flyNext, tile);
            body = flyNext;
        }
        return;
    }

    // --- Normal Movement Logic ---

    if (reached_exit)  return; 

    Point nextPos = body;
    nextPos.move();

    bool isTryingToMove = !(nextPos.getX() == body.getX() && nextPos.getY() == body.getY());
    
    // Spring Charging Logic
    if (activeSpringForce > 0) {
        char nextTile = screen.getCharAt(nextPos);
        if (isTryingToMove && nextTile == Screen::SPRING) {
            activeSpringForce++;
            body = nextPos;
            return;
        }

        else if (isTryingToMove && (nextTile == Screen::WALL || nextTile == Screen::OBSTACLE)) {
            int dx = nextPos.getDiffX();
            int dy = nextPos.getDiffY();
            activateSpring(activeSpringForce, -dx, -dy);
            activeSpringForce = 0;
            return;
        }

        else {
            int pushDx = -body.getDiffX();
            int pushDy = -body.getDiffY();
            activateSpring(activeSpringForce, pushDx, pushDy);
            activeSpringForce = 0;
            return;
        }
    }

    char tile = screen.getCharAt(nextPos);

    if (tile == Screen::WALL) return;

    // Check collision with other player
    if (otherPlayer != nullptr) {
        if (!otherPlayer->reachedExit()) {
            Point otherPos = otherPlayer->getPosition();
            if (nextPos.getX() == otherPos.getX() && nextPos.getY() == otherPos.getY())
                return;
        }
    }

    // Riddle Logic
    if (tile == Screen::RIDDLE) {
        riddleTriggered = true;
        targetPos = nextPos;
        return;
    }

    // Spring Activation Logic
    if (tile == Screen::SPRING) {
        Point probe = nextPos;
        bool hasBackstop = false;
        int limit = 0;
        while(limit < 20) {
            probe.move();
            char probeTile = screen.getCharAt(probe);

            if (probeTile == Screen::WALL || probeTile == Screen::OBSTACLE) {
                hasBackstop = true;
                break;
            }
            if (probeTile != Screen::SPRING) {
                hasBackstop = false;
                break;
            }
            limit++;
        }
        if (hasBackstop) {
            activeSpringForce = 1;
            body = nextPos;
            return;
        }
    }

    if (tryHandleSwitch(nextPos, tile)) return;

    if (tile == Screen::OBSTACLE)
       if (!tryPushObstacle(nextPos, otherPlayer)) return; 

    if (isdigit(tile)) {
       if (tryEnterDoor(nextPos, tile)) return;
       else return; 
    }

    tryCollectItem(nextPos, tile);

    body = nextPos;
}

ItemType Player::handleKeyPressed(char key_pressed) {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
            Direction dir = static_cast<Direction>(index);
            if (dir == Direction::DISPOSE) {
                char itemChar = disposeItem();
                if (itemChar != ' ') {
                    screen.setChar(body, itemChar);
                    if (itemChar == '!') return ItemType::TORCH;
                    if (itemChar == '@') return ItemType::BOMB;
                }
                return ItemType::NONE;
            }
            else {
                body.setDirection((Direction)index);
                return ItemType::NONE;
            }
        }
        ++index;
    }
    return ItemType::NONE;
}

void Player::setPosition(const Point& newPos) {
    body = newPos;
    hasKey = false;
}

void Player::reset() {
    currentItem = ItemType::NONE;
    keysCollected = 0;
    reached_exit = false;
    last_message = "";

    springCyclesLeft = 0;
    springSpeed = 0;
    springDirX = 0;
    springDirY = 0;
    activeSpringForce = 0;
}

void Player::collectItem(char itemChar) {
    if (itemChar == '!') currentItem = ItemType::TORCH;
    else if (itemChar == '@') currentItem = ItemType::BOMB;
    else if (itemChar == 'K') keysCollected++;
}

char Player::disposeItem()
{
	// Drop current item if any, otherwise drop a key if available (priority to items)
    if (currentItem != ItemType::NONE) {
        char itemToDrop = (currentItem == ItemType::TORCH) ? '!' : '@';
        currentItem = ItemType::NONE;
        return itemToDrop;
    }

    if (keysCollected > 0) {
        keysCollected--;
        return Screen::KEY;
    }
    return ' '; 
}

void Player::activateSpring(int force, int dx, int dy) {
    springSpeed = force;
    springCyclesLeft = force * force;
    springDirX = dx;
    springDirY = dy;
}
