#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    std::string message;
    char buffer[1024];
    while (true) {
        std::getline(std::cin, message);
        send(serverSocket, message.c_str(), message.length(), 0);
        int bytesReceived = recv(serverSocket, buffer, 1024, 0);
        std::cout << "Server: " << std::string(buffer, 0, bytesReceived) << std::endl;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}