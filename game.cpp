#include "game.h"
#include <sstream>

Game::Game() : board(6, std::vector<char>(7, ' ')) {}

bool Game::dropPiece(int col, char piece) {
    if (col < 0 || col >= 7 || board[0][col] != ' ') {
        return false;
    }
    for (int row = 5; row >= 0; --row) {
        if (board[row][col] == ' ') {
            board[row][col] = piece;
            return true;
        }
    }
    return false;
}

bool Game::isValidMove(int col) const {
    return col >= 0 && col < 7 && board[0][col] == ' ';
}

bool Game::isBoardFull() const {
    for (int col = 0; col < 7; ++col) {
        if (board[0][col] == ' ') {
            return false;
        }
    }
    return true;
}

bool Game::checkWin(char piece) const {
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            if (board[row][col] == piece) {
                if (checkDirection(row, col, 1, 0, piece) || // vertical
                    checkDirection(row, col, 0, 1, piece) || // horizontal
                    checkDirection(row, col, 1, 1, piece) || // diagonal down-right
                    checkDirection(row, col, 1, -1, piece)) { // diagonal down-left
                    return true;
                }
            }
        }
    }
    return false;
}

bool Game::checkDirection(int row, int col, int rowDir, int colDir, char piece) const {
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        int r = row + i * rowDir;
        int c = col + i * colDir;
        if (r < 0 || r >= 6 || c < 0 || c >= 7 || board[r][c] != piece) {
            return false;
        }
    }
    return true;
}

std::string Game::getBoardAsString() const {
    std::stringstream ss;
    ss << "TABLERO\n";
    int aux = 0;
    for (const auto& row : board) {
        ss << ++aux << ' ';
        for (char cell : row) {
            ss << cell << ' ';
        }
        ss << '\n';
    }
    ss << "  -------------\n";
    ss << "  1 2 3 4 5 6 7\n";
    return ss.str();
}
