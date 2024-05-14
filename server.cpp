#include <iostream>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <arpa/inet.h> 
#include <cstdlib>
#include "game.h"

void handleClient(int clientSocket, sockaddr_in clientAddr);

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
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    srand(time(0)); // Initialize random seed

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        std::thread clientThread(handleClient, clientSocket, clientAddr);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}

void handleClient(int clientSocket, sockaddr_in clientAddr) {

    #include <arpa/inet.h> 

    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);

    std::cout << "Juego nuevo [" << clientIp << ":" << clientPort << "]" << std::endl;

    Game game;
    char buffer[256];
    bool gameOn = true;
    char currentPlayer = 'C';

    send(clientSocket, game.getBoardAsString().c_str(), 256, 0);

    while (gameOn) {
        int col;
        if (currentPlayer == 'C') {
            int bytesReceived = recv(clientSocket, buffer, 256, 0);
            if (bytesReceived <= 0) {
                std::cerr << "Error receiving data or client disconnected" << std::endl;
                close(clientSocket);
                return;
            }
            buffer[bytesReceived] = '\0';
            col = std::stoi(buffer);
            std::cout << "[" << clientIp << ":" << clientPort << "] cliente juega en columna " << col << std::endl;
        } else {
            col = rand() % 7; // Random column for the server
            std::cout << "[" << clientIp << ":" << clientPort << "] server juega en columna " << col << std::endl;
        }

        if (!game.dropPiece(col, currentPlayer)) {
            if (currentPlayer == 'C') {
                std::string errorMsg = "Invalid move. Try again.\n";
                send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            }
            continue;
        }

        send(clientSocket, game.getBoardAsString().c_str(), 256, 0); // Send updated board after each move

        if (game.checkWin(currentPlayer)) {
            std::string winMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] gana " + std::string(1, currentPlayer) + "\n";
            std::string winMsgCliente = "gana " + std::string(1, currentPlayer) + "\n";
            send(clientSocket, winMsgCliente.c_str(), winMsgCliente.size(), 0);
            std::cout << winMsg;
            std::string endGameMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] fin del juego\n";
            std::string endGameMsgCliente = "fin del juego\n";
            send(clientSocket, endGameMsgCliente.c_str(), endGameMsgCliente.size(), 0);
            std::cout << endGameMsg;
            gameOn = false;
        } 
        else if (game.isBoardFull()) {
            std::string drawMsg = "Empate!\n";
            send(clientSocket, drawMsg.c_str(), drawMsg.size(), 0);
            std::cout << drawMsg;
            std::string endGameMsg = "[" + std::string(clientIp) + ":" + std::to_string(clientPort) + "] fin del juego\n";
            send(clientSocket, endGameMsg.c_str(), endGameMsg.size(), 0);
            std::cout << endGameMsg;
            gameOn = false;
        }

        currentPlayer = (currentPlayer == 'C') ? 'S' : 'C';
    }

    close(clientSocket);
}