#include "board.h"

#include <algorithm>

void Board::Point::limit() {
    this->x = (0 <= this->x ? (this->x < BOARD_SIZE ? this->x : BOARD_SIZE - 1) : 0);
    this->y = (0 <= this->y ? (this->y < BOARD_SIZE ? this->y : BOARD_SIZE - 1) : 0);
}

void Board::Board::reset() {
    std::fill(
        &this->board[0][0],
        &this->board[0][0] + sizeof(this->board) / sizeof(this->board[0][0]),
        PIECE_LIST[4]);
}