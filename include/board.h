#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
namespace Board {
const int BOARD_SIZE = 15;
const std::array<std::string, 5> PIECE_LIST = {"─●─", "─○─", "─▲─", "─△─", "─┼─"};

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
    Point() {
        this->x = 0;
        this->y = 0;
    }
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
    bool operator==(Point p) {
        return this->x == p.x and this->y == p.y;
    }

    void limit();
};
const std::array<Point, 4> vectors = {
    Point(1, 0),
    Point(0, 1),
    Point(1, 1),
    Point(-1, 1)};

class Board {
  public:
    std::array<std::array<std::string, BOARD_SIZE>, BOARD_SIZE> board;
    void reset();
    Board() {
        this->reset();
    }
    Board operator=(Board b) {
        Board copied_board;
        copied_board.board = b.board;
        return copied_board;
    }
    // 重载Board类的[]运算符, 使其既可以用来访问元素也可以给元素赋值
    std::string &operator[](Point p) {
        return this->board[p.y][p.x];
    }
};
}; // namespace Board

#endif