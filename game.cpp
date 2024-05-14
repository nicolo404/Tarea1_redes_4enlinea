#include "game.h"
#include <sstream>

Game::Game() {
    resetBoard();
}

void Game::resetBoard() {
    board = std::vector<std::vector<char>>(ROWS, std::vector<char>(COLS, ' '));
}

bool Game::dropPiece(int col, char player) {
    if (col < 0 || col >= COLS || board[0][col] != ' ') {
        return false;
    }
    for (int row = ROWS - 1; row >= 0; --row) {
        if (board[row][col] == ' ') {
            board[row][col] = player;
            return true;
        }
    }
    return false;
}

bool Game::checkWin(char player) {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (checkLine(row, col, 1, 0, player) || 
                checkLine(row, col, 0, 1, player) || 
                checkLine(row, col, 1, 1, player) || 
                checkLine(row, col, 1, -1, player)) {
                return true;
            }
        }
    }
    return false;
}

bool Game::checkLine(int startRow, int startCol, int deltaRow, int deltaCol, char player) {
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        int row = startRow + i * deltaRow;
        int col = startCol + i * deltaCol;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS && board[row][col] == player) {
            count++;
        } else {
            break;
        }
    }
    return count == 4;
}

bool Game::isBoardFull() {
    for (int col = 0; col < COLS; ++col) {
        if (board[0][col] == ' ') {
            return false;
        }
    }
    return true;
}

std::string Game::getBoardAsString() {
    std::ostringstream oss;
    for (const auto& row : board) {
        for (const auto& cell : row) {
            oss << (cell == ' ' ? '.' : cell) << ' ';
        }
        oss << '\n';
    }
    return oss.str();
}




