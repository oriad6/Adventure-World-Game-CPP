#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Point.h"
#include "Rectangle.h"

class Player; // forward declaration

using std::cout;
using std::endl;

class Door {
    int keysRequired;
    int switchesRequired;
	friend class Screen;
public:
	Door(int keys = 0, int switches = 0) : keysRequired(keys), switchesRequired(switches) {}
};

class Screen {
public:
    enum { MAX_X = 80, MAX_Y = 25 };
    static constexpr int MIN_Y = 0;
	static constexpr int MAX_DOORS = 10;
    static constexpr char WALL = 'W';
    static constexpr char KEY = 'K';
    static constexpr char SPACE = ' ';
    static constexpr char RIDDLE = '?';
    static constexpr char TORCH = '!';
    static constexpr char BOMB = '@';
    static constexpr char SPRING = '#';
    static constexpr char OBSTACLE = '*';
    static constexpr char SWITCH_ON = '+';
    static constexpr char SWITCH_OFF = '-';
private:
    char screen[MAX_Y][MAX_X + 1] = { 0 };
    char currScreen[MAX_Y][MAX_X + 1] = { 0 };
    int currentWidth = 0;
    int currentHeight = 0;
    Point legendLocation = Point(0, 0, 0, 0, ' '); //L
    GameRect darkArea;
    Door currentDoorsState[MAX_DOORS];
    int activeSwitchesCount = 0;
    Point startP1 = Point(1, 1, 0, 0, ' ');
    Point startP2 = Point(1, 2, 0, 0, ' ');
	bool showLegend = false;
    int legendStartRow = 0;
    int legendEndRow = 0;

    bool isPixelVisble(int row, int col, const Player& p1, const Player& p2) const;
    void findConnectedObstacle(int x, int y, std::vector<Point>& obstaclePoints, bool visited[MAX_Y][MAX_X]) const;

public:
    Screen();
    bool loadMapFromFile(const std::string& filename);
    void setChar(const Point& p, char c);
    bool handleDoor(char doorChar, int& keysCollected, std::string& msg);
    // void deleteDoor(char doorChar); Not needed currently, maybe we'll implement it later for inside doors
    void draw(const Player& p1, const Player& p2);
    void resetDrawingState();

    char getCharAt(const Point& p) const {
        if (p.getX() < 0 || p.getX() >= currentWidth || p.getY() < 0 || p.getY() >= currentHeight || (p.getY() >= legendStartRow && p.getY() < legendEndRow))
            return WALL;
        return screen[p.getY()][p.getX()];
    }

    bool isWall(const Point& p) const {
        char c = getCharAt(p);
        return c == 'W';
    }

    Point getStartP1() const { return startP1; }
    Point getStartP2() const { return startP2; }

    int getWidth() const { return currentWidth; }
    int getHeight() const { return currentHeight; }

	bool isLegendVisible() const { return showLegend; }

    bool moveObstacle(const Point& pushingPoint, int dx, int dy, int force);
    void handleSwitch(const Point& p);

    Point getLegendLocation() const { return legendLocation; }
    void setDoorConfig(int doorIndex, int keys, int switches);
    void clearDoorConfig();
};
