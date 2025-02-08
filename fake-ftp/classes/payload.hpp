#pragma once

SOCKET openDataConnection(int port) {
    SOCKET tmpDataSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in dataAddr;
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_addr.s_addr = INADDR_ANY;
    dataAddr.sin_port = htons(port);

    bind(tmpDataSocket, (sockaddr*)&dataAddr, sizeof(dataAddr));
    listen(tmpDataSocket, 1);  // Allow one incoming connection

    return tmpDataSocket;
}

void createUsefulBuff(std::string& buff) {
    std::string finalBuff = "";
    for (char& c : buff) {
        if (std::isalnum(c) || c == '\\' || c == '/' || c == '_' || c == ' ' || c == '.' || c == '-' || c == '(' || c == ')' || c == '+' || c == '=' || c == ',' || c == ';' || c == '\'' || c == '!' || c == '$' || c == '&' || c == '@' || c == '^' || c == '~' || c == '[' || c == ']' || c == '{' || c == '}' || c == ':') {
            finalBuff += c;
        }
    }
    buff = finalBuff;
}

std::string reactBasedOnRequest(SOCKET clientSocket, std::string buff, size_t handshakeIndex) {
    createUsefulBuff(buff);
    std::cout << "buff = |" << buff << "|\n";
    if (buff == "PWD") {
        sendToClient(clientSocket, "257 \"/\" is the current directory\r\n");
    }
    else if (buff == "USER wwwqr") {
        sendToClient(clientSocket, "331 Username OK, need password\r\n");
    }
    else if (buff == "PASS wwwqr") {
        sendToClient(clientSocket, "230 Login successful\r\n");
    }
    else if (buff == "SYST") {
        sendToClient(clientSocket, "215 UNIX Type: L8\r\n");
    }
    else if (buff == "FEAT") {
        sendToClient(clientSocket, "211 No additional features\r\n");
    }
    else if (buff == "CWD /") {
        sendToClient(clientSocket, "250 CWD command successful\r\n");
    }
    else if (buff == "TYPE A") {
        sendToClient(clientSocket, "200 Type set to A\r\n");
    }
    else if (buff == "TYPE I") {
        sendToClient(clientSocket, "200 Type set to I\r\n");
    }
    else if (buff == "PASV") {
        size_t dataPort = 2121;
        sendToClient(clientSocket, "227 Entering Passive Mode (127,0,0,1," + std::to_string(dataPort / 256) + "," + std::to_string(dataPort % 256) + ")\r\n");
        dataSocket = openDataConnection(dataPort);
    }
    else if (buff == "LIST -a" || buff == "LIST") {
        sendToClient(clientSocket, "150 Here comes the directory listing\r\n");

        SOCKET dataClient = accept(dataSocket, nullptr, nullptr);
        if (dataClient == INVALID_SOCKET) {
            std::cout << "Invalid dataClient.\n";
            sendToClient(clientSocket, "425 Can't open data connection.\r\n");
        }
        else {
            sendToClient(dataClient, "-rw-r--r--    1 user    group         1286736435445 Feb  7 12:34 passwords.txt\r\n");
            sendToClient(dataClient, "drwxr-xr-x    1 user    group        4096 Feb  7 9:11 fake_folder\r\n");
            shutdown(dataClient, SD_SEND);
            closesocket(dataClient);
            sendToClient(clientSocket, "226 Transfer complete\r\n");
        }
    }
    return "";
}
