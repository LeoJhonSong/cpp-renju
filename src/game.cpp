#include "game.h"

#include "board.h"

#include <array>
#include <iostream>

void Game::Game::reset() {
    this->board.reset();
    this->players[0].last = Board::Point(-1, -1);
    this->players[1].last = Board::Point(-1, -1);
    this->players[Board::BLACK].player.side = Board::BLACK;
    this->players[Board::WHITE].player.side = Board::WHITE;
    this->current = Board::BLACK;
}

void Game::Game::run() {
    Board::Side state;
    // 一共玩两局
    for (int i = 0; i < 2; i++) {
        this->reset();
        do {
            state = this->move();
            this->current = (Board::Side)(this->current ^ 1); // 下一步由另一方走
        } while (state == Board::UNKNOWN);                    // 一直下, 直到出现胜负
    }
    this->score(state);
    this->draw();                                  // FIXME: 这该打印下完最后一步情况及最终结果, 不太一样
    std::swap(this->players[0], this->players[1]); // 下一局黑白方交换
    // TODO: cout输入ENTER进入下一回合?
}

void Game::Game::draw() {
    // TODO: 没打完整, 比如刻度
    for (int i = 0; i < Board::BOARD_SIZE; i++) {
        for (int j = 0; j < Board::BOARD_SIZE; j++) {
            std::cout << this->board.board[i][j];
        }
        std::cout << std::endl;
    }
}

Board::Side Game::Game::move() {
    this->draw();
    while (true) {
        std::array<int, 3> decision = this->players[this->current].player.add_stone(this->board);
        if (decision[2]) { // 当前玩家声明对方触发禁手
            if (this->current == Board::BLACK) {
                std::cout << this->players[Board::WHITE].player.name << "是白方, 没有禁手" << std::endl;
            } else {
                if (this->check_forbidden()) { // 检查黑方是否真的触发禁手
                    return Board::WHITE;
                } else {
                    std::cout << "黑方未形成禁手!" << std::endl;
                }
            }
        } else {
            if (this->board[Board::Point(decision[0], decision[1])] == PIECE)
        }
    }
}

void Game::Game::score(Board::Side side) {
    switch ((int)side) {
        case Board::BLACK:
        case Board::WHITE:
            this->players[(int)side].score += 2;
            break;
        case Board::DRAW:
            this->players[0].score += 1;
            this->players[1].score += 1;
            break;
        default: // 理论上不会进这个分支
            std::cout << "程序出错, 比赛未结束" << std::endl;
            break;
    }
}