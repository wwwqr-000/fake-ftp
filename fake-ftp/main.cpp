#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <thread>
#include <vector>

void sendToClient(SOCKET, std::string);//Pre-load

//Linkers: gdi32, ws2_32

const int port = 21;
std::vector<SOCKET> clients;
SOCKET dataSocket;
std::vector<std::thread> threads;

#include "classes/payload.hpp"

void wait() {
    system("set /p end=");
}

void cls() {
    system("cls");
}

void sendToClient(SOCKET clientSocket, std::string msg) {
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

void clientHandler(SOCKET serverSocket, SOCKET clientSocket) {
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Invalid socket.\n";
    }
    sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &len);
    std::cout << "Client joined (" << inet_ntoa(clientAddr.sin_addr) << ")\n";

    int handShakeCount = 0;

    sendToClient(clientSocket, "220 FTP Server Ready\r\n");

    while (true) {
        ++handShakeCount;
        int recBuff = 3000;
        char buff[recBuff];
        int br = recv(clientSocket, buff, recBuff, 0);
        if (br > 0) {
            std::string msg = std::string(buff, br);
            std::cout << "Server << Client -> " << msg << "\n";
            std::string toClientMsg = reactBasedOnRequest(clientSocket, msg, handShakeCount);

            std::cout << "Server >> Client -> " << toClientMsg << "\n";
        }
        else {
            break;
        }
    }
}

void clientListener(SOCKET serverSocket) {//Create a seperate thread for a incoming client.
    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        threads.emplace_back([serverSocket, clientSocket]() { clientHandler(serverSocket, clientSocket); });
    }
}

void killThreads() {
    for (std::thread& t : threads) {
        t.detach();
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Error 1\n";
        return 1;
    }
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error 2\n";
        return 2;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET) {
        std::cout << "Error 3\n";
        return 3;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Error 4\n";
        return 4;
    }

    std::cout << "Server running on port " << port << "\n";

    threads.emplace_back([serverSocket]() { clientListener(serverSocket); });

    std::cout << "Listening for clients...\n";

    wait();
    killThreads();
    return 0;
}
