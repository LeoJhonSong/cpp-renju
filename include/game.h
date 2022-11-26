#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

#include <array>
#include <vector>

namespace Game {
struct Player_Info {
    Player::Player player;
    unsigned int score = 0;
    Board::Point last = Board::Point(-1, -1);
    Player_Info(Player::Player _player) {
        player = _player;
    }
};
class Game {
  public:
    Board::Board board;
    std::array<Player_Info, 2> players = {
        Player_Info(Player::ManPlayer("You")),
        Player_Info(Player::ComputerPlayer("Computer"))};
    Board::Side current = Board::BLACK;

    void reset();
    void run();
    void draw();
    ///
    ///@brief 黑/白其中一方走一步
    ///
    ///@return Side 返回的是当前棋局状态
    ///
    Board::Side move();
    void score(Board::Side side);
    Board::Side judge();
    Board::Side judge(Board::Board temp_board, Board::Point temp_last);
    bool check_forbidden();
    bool check_forbidden(Board::Board temp_board, Board::Point temp_last);
    bool is_overline(Board::Board board, Board::Point center);
    bool is_double_four(Board::Board board, Board::Point center);
    bool is_double_three(Board::Board board, Board::Point center);
    std::vector<std::string> line_gen(Board::Board board, Board::Point vec, Board::Point center);
    std::vector<int> find_continuous(std::vector<std::string>, unsigned int center);
    std::vector<unsigned int> is_four(Board::Board board, Board::Point vec, Board::Point center);
    std::vector<bool> is_open_three(Board::Board board, Board::Point vec, Board::Point center);
};
}; // namespace Game

#endif