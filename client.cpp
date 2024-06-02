#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
        return 1;
    }

    const char* serverIp = argv[1];
    int serverPort = std::stoi(argv[2]);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return 1;
    }

    char buffer[256];

    // Receive initial message indicating who starts
    int bytesReceived = recv(clientSocket, buffer, 256, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Error receiving data from server or server disconnected" << std::endl;
        close(clientSocket);
        return 1;
    }
    buffer[bytesReceived] = '\0';
    std::cout << buffer << std::endl;

    // Receive initial board state
    bytesReceived = recv(clientSocket, buffer, 256, 0);
    if (bytesReceived <= 0) {
        std::cerr << "Error receiving data from server or server disconnected" << std::endl;
        close(clientSocket);
        return 1;
    }
    buffer[bytesReceived] = '\0';
    std::cout << buffer << std::endl;

    while (true) {
        std::cout << "Indique columna(1-7): ";
        int col;
        std::cin >> col;

        std::string colStr = std::to_string(col-1);
        send(clientSocket, colStr.c_str(), colStr.size(), 0);

        // Receive updated board after client's and server's moves
        bytesReceived = recv(clientSocket, buffer, 256, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error receiving updated board from server or server disconnected" << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << buffer << std::endl;

        // Check for win or draw message
        if (strstr(buffer, "gana") != nullptr || strstr(buffer, "Empate!") != nullptr) {
            std::cout << buffer;
            bytesReceived = recv(clientSocket, buffer, 256, 0); // Receive end game message
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << buffer;
            }
            break;
        }
    }

    close(clientSocket);
    return 0;
}

