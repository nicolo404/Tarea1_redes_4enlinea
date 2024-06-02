#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include "game.h"

void handleClient(int clientSocket, sockaddr_in clientAddr) {
    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);

    std::cout << "Juego nuevo [" << clientIp << ":" << clientPort << "]" << std::endl;

    Game game;
    char buffer[256];
    bool gameOn = true;
    char currentPlayer = (rand() % 2 == 0) ? 'C' : 'S';

    std::string startMsg = "Inicia " + std::string((currentPlayer == 'C') ? "Cliente" : "Servidor") + "\n";
    send(clientSocket, startMsg.c_str(), startMsg.size(), 0);

    if (currentPlayer == 'S') {
        int col = rand() % 7; // Random column for the server
        game.dropPiece(col, 'S');
        std::cout << "[" << clientIp << ":" << clientPort << "] server juega en columna " << col << std::endl;
        send(clientSocket, game.getBoardAsString().c_str(), 256, 0); // Send board state after server's move
        currentPlayer = 'C'; // Switch to client
    } else {
        send(clientSocket, game.getBoardAsString().c_str(), 256, 0); // Send initial empty board
    }

    while (gameOn) {
        if (currentPlayer == 'C') {
            int bytesReceived = recv(clientSocket, buffer, 256, 0);
            if (bytesReceived <= 0) {
                std::cerr << "Error receiving data or client disconnected" << std::endl;
                close(clientSocket);
                return;
            }
            buffer[bytesReceived] = '\0';
            int col = std::stoi(buffer);
            std::cout << "[" << clientIp << ":" << clientPort << "] cliente juega en columna " << col + 1<< std::endl;

            if (!game.dropPiece(col, 'C')) {
                std::string errorMsg = "Invalid move. Try again.\n";
                send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
                continue;
            }

            if (game.checkWin('C')) {
                gameOn = false;
                std::string winMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] gana Cliente\n";
                send(clientSocket, winMsg.c_str(), winMsg.size(), 0);
                std::cout << winMsg;
                break;
            } else if (game.isBoardFull()) {
                gameOn = false;
                std::string drawMsg = "Empate!\n";
                send(clientSocket, drawMsg.c_str(), drawMsg.size(), 0);
                std::cout << drawMsg;
                break;
            }

            currentPlayer = 'S'; // Change turn to server
        }

        if (currentPlayer == 'S') {
            int col = rand() % 7; // Random column for the server
            while (!game.isValidMove(col)) {
                col = rand() % 7; // Ensure valid move
            }
            std::cout << "[" << clientIp << ":" << clientPort << "] server juega en columna " << col+1 << std::endl;
            game.dropPiece(col, 'S');

            if (game.checkWin('S')) {
                gameOn = false;
                std::string winMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] gana Servidor\n";
                send(clientSocket, winMsg.c_str(), winMsg.size(), 0);
                std::cout << winMsg;
                break;
            } else if (game.isBoardFull()) {
                gameOn = false;
                std::string drawMsg = "Empate!\n";
                send(clientSocket, drawMsg.c_str(), drawMsg.size(), 0);
                std::cout << drawMsg;
                break;
            }

            currentPlayer = 'C'; // Change turn to client
        }

        send(clientSocket, game.getBoardAsString().c_str(), 256, 0); // Send updated board after both moves
    }

    std::string endGameMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] fin del juego\n";
    send(clientSocket, endGameMsg.c_str(), endGameMsg.size(), 0);
    std::cout << endGameMsg;

    close(clientSocket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding to port" << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Esperando conexiones en el puerto " << port << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        if (clientSocket == -1) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }

        std::thread(handleClient, clientSocket, clientAddr).detach();
    }

    close(serverSocket);
    return 0;
}
