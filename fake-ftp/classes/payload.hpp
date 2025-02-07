#pragma once

std::string getDataBasedOnOrder(int orderCount) {
    std::string toClientMsg = "";
    switch (orderCount) {
        case 1://Send ok user, need pass
            toClientMsg = "331 Username OK, need password\r\n";
        break;
        case 2://Send login ok
            toClientMsg = "230 Login successful\r\n";
        break;
        case 3://Send type of system
            toClientMsg = "215 UNIX Type: L8\r\n";
        break;
        case 4://Send features of ftp server
            toClientMsg = "211 No additional features\r\n";
        break;
        case 5://Send current dir path
            toClientMsg = "257 \"/\" is the current directory\r\n";
        break;
        case 6://Send confirm ASCII mode
            toClientMsg = "200 Type set to A\r\n";
        break;
        case 7://React to PASV, send server details
            toClientMsg = "227 Entering Passive Mode (192,168,1,1,19,44)\r\n";
        break;
        case 8://React to LIST -a by sending invite msg for sending dirs
            toClientMsg = "150 Here comes the directory listing\r\n";
        break;
        case 9://Send confirm ASCII mode (again)
            toClientMsg = "200 Type set to A\r\n";
        break;

    }

    return toClientMsg;
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
    else if (buff == "PASV") {
        sendToClient(clientSocket, "227 Entering Passive Mode (172,17,240,1,44)\r\n");
    }
    else if (buff == "LIST -a" || buff == "LIST") {
        sendToClient(clientSocket, "150 Here comes the directory listing\r\n");
        sendToClient(clientSocket, "-rw-r--r--    1 user    group         479 Feb  7 12:34 file1.txt\r\n");
        sendToClient(clientSocket, "226 Transfer complete\r\n");
    }
    return "";
}
