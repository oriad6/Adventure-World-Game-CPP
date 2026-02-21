#pragma once

#include "Point.h"
#include "Screen.h"

class Bomb {
    Point position;
    int timer;
    static constexpr int MAX_TICKS = 60;

public:
    Bomb(const Point& pos);
    bool tick();
    void explode(Screen& screen);
    const Point& getposition() const { return position; }
};
