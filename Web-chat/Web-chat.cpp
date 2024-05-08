#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void clientHandler(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        // Send message to other clients
        std::cout << "Received: " << buffer << std::endl;
        send(clientSocket, buffer, bytesReceived, 0);
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listeningSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        return 1;
    }

    if (listen(listeningSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        return 1;
    }

    sockaddr_in target;

    target.sin_family = AF_INET;           // семество адресов - Интернет
    target.sin_port = htons(5001);        // порт сервера
    target.sin_addr.s_addr = inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // IP-адрес сервера
    if (connect(listeningSocket, reinterpret_cast<const sockaddr*>(&target), sizeof(target)) == SOCKET_ERROR)
    {
        std::cerr << "Connection error." << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        SOCKET clientSocket = accept(listeningSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Invalid client socket." << std::endl;
            continue;
        }

        std::thread(clientHandler, clientSocket).detach();
        std::cout << "Client connected!" << std::endl;
    }

    WSACleanup();
    return 0;
}