#ifndef BOARD_H
#define BOARD_H

#include <string>
namespace Board {
const int BOARD_SIZE = 15;
const std::string PIECE_LIST[5] = {"─●─", "─○─", "─▲─", "─△─", "─┼─"};

enum Side : char { // 黑白方及胜负状态的枚举值
    UNKNOWN = -1,
    BLACK = 0,
    WHITE = 1,
    DRAW = 2
};

class Point {
  public:
    int x;
    int y;
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
    Point operator*(int scale) {
        return Point(this->x * scale, this->y * scale);
    }
    Point operator+(Point p) {
        return Point(this->x + p.x, this->y + p.y);
    }
    Point operator-(Point p) {
        return Point(this->x - p.x, this->y - p.y);
    }
};
const Point vector[4] = {
    Point(1, 0),
    Point(0, 1),
    Point(1, 1),
    Point(-1, 1)};

class Board {
  public:
    std::string board[BOARD_SIZE][BOARD_SIZE]{};
    void reset();
    Board() {
        this->reset();
    }
    // 重载Board类的[]运算符
    std::string operator[](Point p) {
        return this->board[p.y][p.x];
    }
};
}; // namespace Board

#endif