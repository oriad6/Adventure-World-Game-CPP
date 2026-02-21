#pragma once

#include <iostream>
#include "console.h"
#include "Direction.h"

class Point {
    int x = 1, y = 1;
    int diff_x = 1, diff_y = 0;
    char ch = '*';
public:
    constexpr Point() {}

    constexpr Point(int x1, int y1, int diffx, int diffy, char c) :
        x(x1), y(y1), diff_x(diffx), diff_y(diffy), ch(c) {
    }

    void draw() const {
        draw(ch);
    }

    void draw(char c) const {
        gotoxy(x, y);
        std::cout << c;
    }
    void move();
    void setDirection(Direction dir);
    int getX() const {return x;}
    int getY() const {return y;}

    int getDiffX() const {return diff_x;}
    int getDiffY() const { return diff_y; }
};
