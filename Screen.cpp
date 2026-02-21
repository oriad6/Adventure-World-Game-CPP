#include "Screen.h"
#include "console.h"
#include "Player.h"
#include <cstring>


Screen::Screen() : darkArea(0, 0, 0, 0) {
    // Initialize the screen and currScreen arrays with SPACE characters
    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            screen[i][j] = SPACE;
            currScreen[i][j] = 0;
        }
        screen[i][MAX_X] = '\0'; // Null-terminate each row
        currScreen[i][MAX_X] = '\0'; // Null-terminate each row
    }
}

void Screen::setChar(const Point& p, char c) {
    if (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y) {
        screen[p.getY()][p.getX()] = c;
        currScreen[p.getY()][p.getX()] = c;
    }
    gotoxy(p.getX(), p.getY());
    cout << c;
}

void Screen::draw(const Player& p1, const Player& p2) { // this function was edited into this version after consulting with Gemini
    for (int row = 0; row < currentHeight; ++row) {
        for (int col = 0; col < currentWidth; ++col) {
            char targetChar = ' ';

            if (isPixelVisble(row, col, p1, p2)) {
                if (!p1.reachedExit() && p1.getPosition().getX() == col && p1.getPosition().getY() == row)
                    targetChar = '$'; // Draw Player 1

                else if (!p2.reachedExit() && p2.getPosition().getX() == col && p2.getPosition().getY() == row)
                    targetChar = '&'; // Draw Player 2
                else
                    targetChar = screen[row][col];
            }
            if (targetChar != currScreen[row][col]) {
                gotoxy(col, row);
                cout << targetChar;
                currScreen[row][col] = targetChar;
            }
        }
    }
    gotoxy(0, MAX_Y + 1);
}

void Screen::resetDrawingState() {
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            currScreen[i][j] = 0; // reset currScreen to force redraw
        }
    }
}

bool Screen::isPixelVisble(int row, int col, const Player& p1, const Player& p2) const {
    // Check if the pixel is inside the dark area
    bool inDarkArea = darkArea.contains(col, row);
    if (!inDarkArea) return true;

    // --- Check Player 1 ---
	Point pos1 = p1.getPosition();
	// Calculate distance from player 1 to the pixel
	int dx1 = std::abs(pos1.getX() - col);
	int dy1 = std::abs(pos1.getY() - row);
    // Check if player 1 has a torch and is inside the dark area
    if (p1.hasTorch() && darkArea.contains(pos1)) return true;
    if (dx1 <= 2 && dy1 <= 2) return true;

	// --- Check Player 2 ---
	Point pos2 = p2.getPosition();
	int dx2 = std::abs(pos2.getX() - col);
	int dy2 = std::abs(pos2.getY() - row);
    // Check if player 2 has a torch and is inside the dark area
    if (p2.hasTorch() && darkArea.contains(pos2)) return true;
	if (dx2 <= 2 && dy2 <= 2) return true;
	// Pixel is not visible to either player
    return false;
}

bool Screen::handleDoor(char doorChar, int& keysCollected, std::string& msg) {
    if (!isdigit(doorChar)) return false; // Not a door
    int index = doorChar - '0'; // Convert char to index
    Door& doorState = currentDoorsState[index];

    while (doorState.keysRequired > 0 && keysCollected > 0) {
        doorState.keysRequired--;
        keysCollected--;
    }

    int missingSwitches = doorState.switchesRequired - activeSwitchesCount;
	if (missingSwitches < 0) missingSwitches = 0;

    if (doorState.keysRequired == 0 && activeSwitchesCount >= doorState.switchesRequired) {
        return true; // Door can be opened
    }

    msg = "Door " + std::to_string(index) + " Locked! requires: ";
    if (doorState.keysRequired > 0) {
        msg += std::to_string(doorState.keysRequired) + " keys ";
    }
	if (activeSwitchesCount < doorState.switchesRequired) {
		if (doorState.keysRequired > 0)
			msg += "and ";
		msg += std::to_string(missingSwitches) + " active switches ";
	}
    return false; // Door cannot be opened
}

void Screen::findConnectedObstacle(int x, int y, std::vector<Point>& obstaclePoints, bool visited[MAX_Y][MAX_X]) const {
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) return;
    if (visited[y][x]) return;
    if (screen[y][x] != OBSTACLE) return;

    visited[y][x] = true;
    obstaclePoints.push_back(Point(x, y, 0, 0, ' '));

    findConnectedObstacle(x + 1, y, obstaclePoints, visited);
    findConnectedObstacle(x - 1, y, obstaclePoints, visited);
    findConnectedObstacle(x, y + 1, obstaclePoints, visited);
    findConnectedObstacle(x, y - 1, obstaclePoints, visited);
}

bool Screen::moveObstacle(const Point& pushingPoint, int dx, int dy, int force) {
    std::vector<Point> obstaclePoints;
    bool visited[MAX_Y][MAX_X] = { false };

    findConnectedObstacle(pushingPoint.getX(), pushingPoint.getY(), obstaclePoints, visited);

    if ((int)obstaclePoints.size() > force)
        return false;

    for (const auto& p : obstaclePoints) {
        int targetX = p.getX() + dx;
        int targetY = p.getY() + dy;

        if (targetX < 0 || targetX >= MAX_X || targetY < 0 || targetY >= MAX_Y)
            return false;

        char targetTile = screen[targetY][targetX];

        if (targetTile != SPACE && targetTile != OBSTACLE)
            return false;

        if (targetTile == OBSTACLE && !visited[targetY][targetX])
            return false;
    }

    for (const auto& p : obstaclePoints)
        setChar(p, SPACE);

    for (const auto& p : obstaclePoints) {
        Point newPos(p.getX() + dx, p.getY() + dy, 0, 0, ' ');
        setChar(newPos, OBSTACLE);
    }

    return true;
}

void Screen::handleSwitch(const Point& p) {
	char currentTile = getCharAt(p);
    if (currentTile == SWITCH_OFF) {
		setChar(p, SWITCH_ON);
		activeSwitchesCount++;
	}
    else if (currentTile == SWITCH_ON) {
        setChar(p, SWITCH_OFF);
        activeSwitchesCount--;
    }
}

bool Screen::loadMapFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    activeSwitchesCount = 0;
    for (int k = 0; k < MAX_DOORS; k++) {
        currentDoorsState[k] = { 0, 0 };
    }

    startP1 = Point(1, 1, 0, 0, ' ');
    startP2 = Point(1, 2, 0, 0, ' ');

    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            screen[i][j] = SPACE;
            currScreen[i][j] = 0;
        }
        screen[i][MAX_X] = '\0';
    }

    legendLocation = Point(0, 0, 0, 0, ' ');
	legendStartRow = -1; 
    legendEndRow = -1;
    showLegend = false;
	
    std::vector<Point> darkMarkers;

    currentWidth = 0;
    currentHeight = 0;

    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < MAX_Y) {
        int len = min((int)line.length(), (int)MAX_X);

        if (len > currentWidth) {
            currentWidth = len;
        }

        for (int col = 0; col < len; ++col) {
            char c = line[col];

            if (c == '$') {
                startP1 = Point(col, row, 0, 0, ' ');
                c = SPACE;
            }
            else if (c == '&') {
                startP2 = Point(col, row, 0, 0, ' ');
                c = SPACE;
            }
            else if (c == 'L') {
                int fixedX = col;
                if (row + 3 <= MAX_Y) {
                    int fixedX = col;
                    if (fixedX > 60) 
						fixedX = 60;
                    
                    legendLocation = Point(fixedX, row, 0, 0, ' ');
                    legendStartRow = row;
                    legendEndRow = row + 3;
                    showLegend = true;
				}
                c = SPACE;
            }
            //Logic for Big D (Wall) and Small d (Space)
            else if (c == 'D') {
                darkMarkers.push_back(Point(col, row, 0, 0, ' '));
                c = WALL;
            }
            else if (c == 'd') {
                darkMarkers.push_back(Point(col, row, 0, 0, ' '));
                c = SPACE;
            }

            screen[row][col] = c;
        }
        row++;
    }
    file.close();
    currentHeight = row;

    if (darkMarkers.size() == 2) {
        int x1 = min(darkMarkers[0].getX(), darkMarkers[1].getX());
        int y1 = min(darkMarkers[0].getY(), darkMarkers[1].getY());
        int x2 = max(darkMarkers[0].getX(), darkMarkers[1].getX());
        int y2 = max(darkMarkers[0].getY(), darkMarkers[1].getY());

        darkArea = GameRect(y1, x1, y2, x2);
    }
    else {
        darkArea = GameRect(-1, -1, -1, -1);
    }
    return true;
}
   
    void Screen::setDoorConfig(int doorIndex, int keys, int switches) {
        if (doorIndex >= 0 && doorIndex < MAX_DOORS)
            currentDoorsState[doorIndex] = Door(keys, switches);
    }

    void Screen::clearDoorConfig() {
        for (int i = 0; i < MAX_DOORS; ++i)
            currentDoorsState[i] = Door(0, 0);
    }
