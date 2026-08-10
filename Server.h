#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include <string>

class Server {
    SOCKET serverSocket;
    sockaddr_in server, client;
    int clientLen;
    ULONGLONG startTick;
public:
    Server(int port);
    ~Server();
    std::string processCommand(const std::string& command);
    std::string receiveMessage();
    void run();
    void logToFile(const std::string& entry);
    void send(const std::string& message);
};

#endif // SERVER_H
