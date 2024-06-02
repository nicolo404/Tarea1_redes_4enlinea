#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>

class Game {
public:
    Game();
    bool dropPiece(int col, char piece);
    bool isValidMove(int col) const;
    bool checkWin(char piece) const;
    bool isBoardFull() const;
    std::string getBoardAsString() const;

private:
    std::vector<std::vector<char>> board;
    bool checkDirection(int row, int col, int rowDir, int colDir, char piece) const;
};

#endif
