#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

#include <array>
#include <vector>

namespace Game {
struct Player_Info {
    Player::Player* player; // 存指针或引用才可以有虚函数调用机制
    unsigned int score = 0;
    Board::Point last = Board::Point(-1, -1);
    Player_Info(Player::Player* _player) {
        player = _player;
    }
};
class Game {
    private:
    Player::ManPlayer player1 = Player::ManPlayer("You");
    Player::ComputerPlayer player2 = Player::ComputerPlayer("Computer");
  public:
    Board::Board board;
    std::array<Player_Info, 2> players = {
        Player_Info(&player1),
        Player_Info(&player2)};
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
    ///
    ///@brief 判定给定棋盘胜负情况
    ///
    ///@param board 给定棋盘
    ///@param last 给定最后一子. 其实只需要判断这最后一子所属一方是否胜利, 因为每走一步都会检查一次胜负情况, 另一方胜负在上一步时检查过了.
    ///@return Board::Side UNKNOWN未分胜负/BLACK黑方胜/WHITE白方胜/DRAW平局
    ///
    Board::Side judge(Board::Board board, Board::Point last);
    Board::Side judge();
    ///
    ///@brief 检查黑方是否触发禁手. 有三种可能调用情况:
    ///1. 电脑执白时调用检查黑方是否触发禁手
    ///2. 电脑执黑时试下一子后调用进行自检
    ///3. 玩家执白时声明黑方触发禁手, 游戏实例自调用进行检查
    ///@param board 指定棋盘
    ///@param last 指定最后落的子
    ///@return true 黑方长连/双四/双三
    ///@return false 黑方未触发禁手/触发了但同时形成连五获胜
    ///
    bool check_forbidden(Board::Board board, Board::Point last);
    bool check_forbidden();
    ///
    ///@brief 检查指定点是否触发长连禁手
    ///
    ///@param board 指定棋盘
    ///@param center 指定落子点
    ///@return true 在该点落子会触发长连禁手
    ///@return false 不会触发
    ///
    bool is_overline(Board::Board board, Board::Point center);
    ///
    ///@brief 检查指定点是否触发双四禁手
    ///
    ///@param board 指定棋盘
    ///@param center 指定落子点
    ///@return true 在该点落子会触发双四禁手
    ///@return false 不会触发
    ///
    bool is_double_four(Board::Board board, Board::Point center);
    ///
    ///@brief 检查指定点是否触发双三禁手
    ///
    ///@param board 指定棋盘
    ///@param center 指定落子点
    ///@return true 在该点落子会触发双三禁手
    ///@return false 不会触发
    ///
    bool is_double_three(Board::Board board, Board::Point center);
    ///
    ///@brief 截取指定棋盘上经过指定点的指定方向上所有点, 并将指定点在该vector中下标记录至c
    ///
    ///@param board 指定棋盘
    ///@param vec 指定方向
    ///@param center 指定点
    ///@param c 指定点在截取到的点的数组中下标
    ///@return std::vector<std::string> 截取到的点的vector
    ///
    std::vector<std::string> line_gen(Board::Board board, Board::Point vec, Board::Point center, int* c);
    ///
    ///@brief 找到指定点棋子附近连续相同颜色棋子的左右边界与指定点棋子的相对距离
    ///
    ///@param line 给定棋子序列
    ///@param center 指定点棋子的下标
    ///@return std::array<int, 2> 最左, 最右相同色棋子与指定点相对距离
    ///
    std::array<int, 2> find_continuous(std::vector<std::string> line, unsigned int center);
    ///
    ///@brief 检查指定点指定方向上是否有黑子活四或冲四
    ///
    ///@param board 指定棋盘
    ///@param vec 指定方向
    ///@param center 指定点
    ///@return 2 活四
    ///@return 1 冲四
    ///@return 0 两者都不是
    ///
    unsigned int is_four(Board::Board board, Board::Point vec, Board::Point center);
    ///
    ///@brief 检查指定点指定方向上是否有黑子活三
    ///
    ///@param board 指定棋盘
    ///@param vec 指定方向
    ///@param center 指定点
    ///@return true 活三
    ///@return false 不是活三
    ///
    bool is_open_three(Board::Board board, Board::Point vec, Board::Point center);
};
}; // namespace Game

#endif