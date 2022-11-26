#include "board.h"

#include <algorithm>

void Board::Board::reset() {
    std::fill(
        &this->board[0][0],
        &this->board[0][0] + sizeof(this->board) / sizeof(this->board[0][0]),
        Board::PIECE_LIST[4]);
}