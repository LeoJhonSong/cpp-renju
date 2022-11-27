
#include "player.h"

#include "board.h"
#include "game.h"

#include <array>
#include <iostream>

// 其实只能伪随机
int rand(int start, int end) {
    int x = end + 1;
    while (x > end) { x = start + std::rand() / ((RAND_MAX + 1u) / (end - start + 1)); }
    return x;
}

std::array<int, 3> Player::Player::add_stone(Game::Game game) {
    return (std::array<int, 3>){0, 0, 0};
}

std::array<int, 3> Player::ManPlayer::add_stone(Game::Game game) {
    std::cout << "请输入想落子处横, 纵坐标 (如12 3): ";
    if (this->side == Board::WHITE) {
        std::cout << "(如果你认为黑方触发了禁手, 输入100 100) ";
    }
    std::cout << std::endl;
    int x, y;
    std::cin >> x >> y;
    if (x == 100 and y == 100) {
        return (std::array<int, 3>){0, 0, 1};
    } else {
        return (std::array<int, 3>){x - 1, y - 1, 0};
    }
}

std::array<int, 3> Player::ComputerPlayer::add_stone(Game::Game game) {
    if (this->side == Board::WHITE) { // 当前是白方
        if (game.check_forbidden()) { // 黑方触发禁手
            return (std::array<int, 3>){0, 0, 1};
        } else {
            while (true) {
                Board::Point try_here = game.players[Board::BLACK].last + Board::Point(rand(-3, 3), rand(-3, 3));
                try_here.limit();
                if (game.board[try_here] == Board::PIECE_LIST[4]) { return (std::array<int, 3>){try_here.x, try_here.y, 0}; }
            }
        }
    } else {                                                           // 当前是黑方
        if (game.players[Board::BLACK].last == Board::Point(-1, -1)) { // 黑方还没有下过子的话直接下在天元处
            return (std::array<int, 3>){Board::BOARD_SIZE / 2 - 1, Board::BOARD_SIZE / 2 - 1, 0};
        } else {
            while (true) { // 找到一个没子且不会形成禁手的地方下
                Board::Board temp_board = game.board;
                Board::Point try_here = game.players[Board::WHITE].last + Board::Point(rand(-3, 3), rand(-3, 3));
                try_here.limit();
                if (temp_board[try_here] == Board::PIECE_LIST[4]) {
                    temp_board[try_here] = Board::PIECE_LIST[Board::BLACK];
                    if (not game.check_forbidden(temp_board, try_here)) {
                        return (std::array<int, 3>){try_here.x, try_here.y, 0};
                    }
                }
            }
        }
    }
}