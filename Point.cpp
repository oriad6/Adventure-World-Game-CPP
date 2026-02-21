#include "Point.h"
#include "Screen.h"

void Point::move() {
    int next_x = x + diff_x;
    int next_y = y + diff_y;
    if (next_x >= 0 && next_x < Screen::MAX_X) {
        x = next_x;
    }
    if (next_y >= Screen::MIN_Y && next_y < Screen::MAX_Y) {
        y = next_y;
    }
}

void Point::setDirection(Direction dir) {
    switch (dir) {
        case Direction::UP:
            diff_x = 0;
            diff_y = -1;
            break;
        case Direction::RIGHT:
            diff_x = 1;
            diff_y = 0;
            break;
        case Direction::DOWN:
            diff_x = 0;
            diff_y = 1;
            break;
        case Direction::LEFT:
            diff_x = -1;
            diff_y = 0;
            break;
        case Direction::STAY:
            diff_x = 0;
            diff_y = 0;
            break;
    }
}
