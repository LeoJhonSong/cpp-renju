#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

#include <array>
#include <string>

// 为解决Player::Player与Game::Game循环引用问题, 进行前置声明
namespace Game {
class Game;
}

namespace Player {
class Player {
  public:
    std::string name;
    Board::Side side = Board::BLACK;
    bool is_computer = false;
    Player(std::string name = "Someone") {
        this->name = name;
    }

    ///
    ///@brief 玩家落下一子
    ///
    ///@param board 用于决策在哪里下子的棋盘
    ///@return std::array<int, 3> 返回落子x, y及黑方是否触发禁手 (只有玩家为白方时可为真)
    ///
    virtual std::array<int, 3> add_stone(Game::Game game); // 这个函数不写函数体, 在子类中实现
};

class ManPlayer : public Player {
  public:
    ManPlayer(std::string name) : Player(name){};
    std::array<int, 3> add_stone(Game::Game game); // return: x, y, is_forbidden
};
class ComputerPlayer : public Player {
  public:
    ComputerPlayer(std::string name) : Player(name){};
    std::array<int, 3> add_stone(Game::Game game); // return: x, y, is_forbidden
};
}; // namespace Player

#endif