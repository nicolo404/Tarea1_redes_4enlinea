#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>

class Game {
public:
    Game();
    bool dropPiece(int col, char player);
    bool checkWin(char player);
    bool isBoardFull();
    std::string getBoardAsString();
    void resetBoard();

private:
    std::vector<std::vector<char>> board;
    const int ROWS = 6;
    const int COLS = 7;
    bool checkLine(int startRow, int startCol, int deltaRow, int deltaCol, char player);
};

#endif // GAME_H
