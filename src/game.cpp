#include "game.h"

#include "board.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

void Game::Game::reset() {
    this->board.reset();
    this->players[0].last = Board::Point(-1, -1);
    this->players[1].last = Board::Point(-1, -1);
    this->players[Board::BLACK].player->side = Board::BLACK;
    this->players[Board::WHITE].player->side = Board::WHITE;
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
        this->score(state);
        std::cout << "───────────────────────────────────────────────" << std::endl;
        std::cout << this->players[state].player->name << "获胜!" << std::endl;
        std::swap(this->players[0], this->players[1]); // 下一局黑白方交换
        std::cout << "按下回车进入下一局" << std::endl;
        std::cout << "───────────────────────────────────────────────" << std::endl;
        getchar();
        getchar();
    }
    std::cout << "游戏结束!" << std::endl;
}

void Game::Game::draw() {
    for (int i = 0; i < 50; i++) { std::cout << std::endl; }
    std::cout << "───────────────────────────────────────────────" << std::endl;
    std::cout << "(黑) " << this->players[Board::BLACK].player->name << ": " << this->players[Board::BLACK].score << "分, (白) " << this->players[Board::WHITE].player->name << ": " << this->players[Board::WHITE].score << "分" << std::endl;
    std::cout << "当前: " << this->players[this->current].player->name << std::endl;
    Board::Board board = this->board;
    for (int i = 0; i < 2; i++) {
        // 将最后落的棋子变成三角形
        if (not(this->players[i].last == Board::Point(-1, -1))) {
            board[this->players[i].last] = Board::PIECE_LIST[i + 2];
        }
    }
    for (int i = 0; i < Board::BOARD_SIZE; i++) {
        std::cout.width(2);
        std::cout << i + 1;
        for (int j = 0; j < Board::BOARD_SIZE; j++) {
            std::cout << board.board[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "  ";
    for (int i = 1; i < 16; i++) {
        std::cout.width(2);
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

Board::Side Game::Game::move() {
    this->draw();
    while (true) {
        std::array<int, 3> decision = this->players[this->current].player->add_stone(*this);
        if (decision[2]) { // 当前玩家声明对方触发禁手
            if (this->current == Board::BLACK) {
                std::cout << this->players[Board::WHITE].player->name << "是白方, 没有禁手" << std::endl;
            } else {
                if (this->check_forbidden()) { // 检查黑方是否真的触发禁手
                    std::cout << "黑方" << this->players[Board::BLACK].player->name << "触发禁手!" << std::endl;
                    return Board::WHITE;
                } else {
                    std::cout << "黑方未形成禁手!" << std::endl;
                }
            }
        } else {
            if (this->board[Board::Point(decision[0], decision[1])] == Board::PIECE_LIST[4]) { // 如果这里本来没有棋子的话
                this->board[Board::Point(decision[0], decision[1])] = Board::PIECE_LIST[(int)(this->current == Board::WHITE)];
                this->players[this->current].last = Board::Point(decision[0], decision[1]);
                return this->judge();
            } else {
                std::cout << "该位置已有子!" << std::endl;
            }
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

Board::Side Game::Game::judge(Board::Board board, Board::Point last) {
    Board::Side side = (Board::Side)std::distance(Board::PIECE_LIST.begin(), std::find(Board::PIECE_LIST.begin(), Board::PIECE_LIST.end(), board[last]));
    for (int i = 0; i < Board::vectors.size(); i++) {
        int c;
        std::vector<std::string> line = this->line_gen(board, Board::vectors[i], last, &c);
        std::array<int, 2> edges = this->find_continuous(line, c);
        int count = edges[1] - edges[0] + 1;
        if (count == 5 or (side == Board::WHITE and count > 5)) { return side; } // >5对于黑方是禁手, 但对于白方是胜利
    }
    for (int i = 0; i < Board::BOARD_SIZE; i++) {
        for (int j = 0; j < Board::BOARD_SIZE; j++) {
            if (board[Board::Point(i, j)] == Board::PIECE_LIST[4]) { return Board::UNKNOWN; } // 如果棋盘没满, 最后落子方也没有连五则胜负未分
        }
    }
    return Board::DRAW; // 剩下的情况只可能是棋盘下满了也未分胜负, 即平局
}

Board::Side Game::Game::judge() {
    return this->judge(this->board, this->players[this->current].last);
}

bool Game::Game::check_forbidden(Board::Board board, Board::Point last) {
    Board::Side winner = this->judge(board, last);
    if (winner == Board::BLACK) { return false; } // 黑方连五与禁手同时形成时禁手失效
    else if (winner == Board::WHITE) {
        return false; // 理论上不会出现winner为白方
    } else {
        if (this->is_overline(board, last)) {
            return true;
        } else if (this->is_double_four(board, last)) {
            return true;
        } else {
            return this->is_double_three(board, last);
        }
    }
}

bool Game::Game::check_forbidden() {
    return this->check_forbidden(this->board, this->players[Board::BLACK].last);
}

bool Game::Game::is_overline(Board::Board board, Board::Point center) {
    for (int i = 0; i < Board::vectors.size(); i++) {
        int c;
        std::vector<std::string> line = this->line_gen(board, Board::vectors[i], center, &c);
        std::array<int, 2> edges = this->find_continuous(line, c);
        if (edges[1] - edges[0] + 1 > 5) {
            // std::cout << "黑方" << this->players[Board::BLACK].player->name << "触发长连禁手" << std::endl;
            return true;
        }
    }
    return false;
}

bool Game::Game::is_double_four(Board::Board board, Board::Point center) {
    unsigned int count = 0;
    for (int i = 0; i < Board::vectors.size(); i++) {
        if (this->is_four(board, Board::vectors[i], center) > 0) { count++; }
        if (count > 1) {
            // std::cout << "黑方" << this->players[Board::BLACK].player->name << "触发双四禁手" << std::endl;
            return true;
        }
    }
    return false;
}

bool Game::Game::is_double_three(Board::Board board, Board::Point center) {
    unsigned int count = 0;
    for (int i = 0; i < Board::vectors.size(); i++) {
        if (this->is_open_three(board, Board::vectors[i], center)) { count++; }
        if (count > 1) {
            // std::cout << "黑方" << this->players[Board::BLACK].player->name << "触发双三禁手" << std::endl;
            return true;
        }
    }
    return false;
}

std::vector<std::string> Game::Game::line_gen(Board::Board board, Board::Point vec, Board::Point center, int *c) {
    std::vector<std::string> line;
    int step = 1;
    Board::Point current;
    while (true) {
        step -= 1;
        current = center + vec * step;
        if (0 <= current.x and current.x <= 14 and 0 <= current.y and current.y <= 14) {
            line.insert(line.begin(), board[current]);
        } else {
            break;
        }
    }
    *c = line.size() - 1;
    step = 0;
    while (true) {
        step += 1;
        current = center + vec * step;
        if (0 <= current.x and current.x <= 14 and 0 <= current.y and current.y <= 14) {
            line.push_back(board[current]);
        } else {
            break;
        }
    }
    return line;
}

std::array<int, 2> Game::Game::find_continuous(std::vector<std::string> line, unsigned int center) {
    int left = center;
    int right = center;
    while (0 <= left - 1 and left - 1 < line.size() and line[left - 1] == line[center]) { left--; }
    while (0 <= right + 1 and right + 1 < line.size() and line[right + 1] == line[center]) { right++; }
    std::array<int, 2> edges = {left - (int)center, right - (int)center};
    return edges;
}

unsigned int Game::Game::is_four(Board::Board board, Board::Point vec, Board::Point center) {
    // 在两侧最近的空处下黑子都能连五就是活四, 只有一处能连五就是冲四, 否则两者都不是
    int c;
    std::vector<std::string> line = this->line_gen(board, vec, center, &c);
    unsigned int count = 0;
    std::array<int, 2> relative_distances = this->find_continuous(line, c);
    for (int i = 0; i < 2; i++) {
        int neighbour = c + relative_distances[i] + (i == 0 ? -1 : 1); // 连子最左/右旁边一个位置
        if (0 <= neighbour and neighbour < line.size() and line[neighbour] == Board::PIECE_LIST[4]) {
            std::vector<std::string> temp_line = line;
            temp_line[neighbour] = Board::PIECE_LIST[Board::BLACK];
            std::array<int, 2> edges = this->find_continuous(temp_line, c);
            if (edges[1] - edges[0] + 1 != 5) { continue; } // 甚至无法连五 (长连不算)
            // 能连五, 检查刚落的黑子是否会触发禁手
            Board::Board temp_board = board;
            temp_board[center + vec * (neighbour - c)] = board[center];
            if (not this->check_forbidden(temp_board, center + vec * (neighbour - c))) { count++; }
        }
    }
    return count;
}

bool Game::Game::is_open_three(Board::Board board, Board::Point vec, Board::Point center) {
    // 在两侧最近的空处之一下黑子能形成活四就是活三
    int c;
    std::vector<std::string> line = this->line_gen(board, vec, center, &c);
    std::array<int, 2> relative_distances = this->find_continuous(line, c);
    for (int i = 0; i < 2; i++) {
        int neighbour = c + relative_distances[i] + (i == 0 ? -1 : 1); // 连子最左/右旁边一个位置
        if (0 <= neighbour and neighbour < line.size() and line[neighbour] == Board::PIECE_LIST[4]) {
            Board::Board temp_board = board;
            temp_board[center + vec * (neighbour - c)] = board[center];
            if (this->is_four(temp_board, vec, center) == 2) {
                // 能形成活四, 检查刚落的黑子是否会触发禁手
                if (not this->check_forbidden(temp_board, center + vec * (neighbour - c))) { return true; }
            }
        }
    }
    return false;
}