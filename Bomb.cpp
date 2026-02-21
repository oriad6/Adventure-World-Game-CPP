#include "Bomb.h"
#include <algorithm>
#include <cmath>

Bomb::Bomb(const Point& pos): position(pos), timer(MAX_TICKS) {}

bool Bomb::tick()
{
    timer--;
    return timer <= 0; // Explode when reachig to 0
}

void Bomb::explode(Screen& screen) {
    int bx = position.getX();
    int by = position.getY();

    for (int y = by - 3; y <= by + 3; ++y) {
        for (int x = bx - 3; x <= bx + 3; ++x) {
            if (x < 0 || x >= Screen::MAX_X || y < 0 || y >= Screen::MAX_Y) // out of bounds
                continue;
            Point target(x, y, 0, 0, ' ');
            char ch = screen.getCharAt(target);

            if (ch == Screen::SPACE) continue; // nothing to destroy

            int dist = max(abs(x - bx), abs(y - by));

            if (ch == Screen::WALL) {
                if (dist <= 1)
                    screen.setChar(target, Screen::SPACE);
            }
            else
                screen.setChar(target, Screen::SPACE);
        }
    }
}
