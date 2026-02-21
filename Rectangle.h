#pragma once
#include "Point.h"

struct GameRect {
// bounds
int top = 0, left = 0, bottom = 0, right = 0;

GameRect(int t, int l, int b, int r) :
	top(t), left(l), bottom(b), right(r) {}

// Default constructor
GameRect() = default;

// check if a point is inside the rectangle
bool contains(const Point& p) const {
	return p.getX() >= left && p.getX() <= right &&
		p.getY() >= top && p.getY() <= bottom;
}

// check if the coordinates are inside the rectangle
bool contains(int x, int y) const {
	return x >= left && x <= right &&
		y >= top && y <= bottom;
}
};