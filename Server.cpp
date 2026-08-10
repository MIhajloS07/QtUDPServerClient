#include "Server.h"
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // Link against the Winsock library
#define BUFFER_SIZE 512
#define _WINSOCK_DEPRECATED_NO_WARNINGS // Disable deprecation warnings for Winsock functions
#define _CRT_SECURE_NO_WARNINGS

Server::Server(int port) {
    WSAData wsa; // Declare a WSAData structure to hold information about the Windows Sockets implementation
    int result = WSAStartup(MAKEWORD(2, 2), &wsa); // Initialize Winsock
    if (result != 0) {
        throw std::runtime_error("WSAStartup failed"); // Throw an exception if Winsock initialization fails
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup(); // Clean up Winsock resources if socket creation fails
        throw std::runtime_error("socket failed"); // Throw an exception if socket creation fails
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET; // Set the address family to IPv4
    server.sin_addr.s_addr = INADDR_ANY; // Set the server to listen on any available network interface
    server.sin_port = htons(port); // Set the port number for the server

    // Bind the socket to the specified port
    if (bind(serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(serverSocket); // Close the server socket if binding fails
        WSACleanup(); // Clean up Winsock resources
        throw std::runtime_error("bind failed"); // Throw an exception if binding the socket fails
    }

    memset(&client, 0, sizeof(client));
    clientLen = sizeof(client); // Set the size of the client address structure
    startTick = GetTickCount64();
}

Server::~Server() {
    closesocket(serverSocket); // Close the server socket
    WSACleanup(); // Clean up Winsock resources
}

std::string Server::processCommand(const std::string& command) {
    if (command == "/ping") {
        return "pong"; // If the received message is "/ping", return "pong"
    }
    else if (command.rfind("/send ", 0) == 0) {
        std::string messageToSend = command.substr(6); // Extract the message to send after "/send "
        return "Server received: " + messageToSend; // Return the received message
    }
    else if (command == "/time") {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buf[64];
        sprintf_s(buf, "%02d:%02d:%02d",
                  st.wHour, st.wMinute, st.wSecond);
        return std::string("Current time: ") + buf;
    }
    else if (command == "/date") {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buf[64];
        sprintf_s(buf, "%02d:%02d:%04d",
                  st.wDay, st.wMonth, st.wYear);
        return std::string("Current date: ") + buf;
    }
    else if (command == "/datetime"){
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buf[64];
        sprintf_s(buf, "%02d-%02d-%04d %02d:%02d:%02d",
                  st.wDay, st.wMonth, st.wYear,
                  st.wHour, st.wMinute, st.wSecond);
        return std::string("Current datetime: ") + buf;
    }
    else if (command == "/uptime"){
        ULONGLONG now = GetTickCount64();
        ULONGLONG diff = (now - startTick) / 1000; // seconds
        char buf[64];
        sprintf_s(buf, "%llu seconds", diff);
        return std::string("Server uptime: ") + buf;
    }
    else if (command == "/ip"){
        return std::string("Client IP address:") + inet_ntoa(client.sin_addr);
    }
    else if (command == "/exit") {
        return "Goodbye!"; // If the received message is "/exit", return "Goodbye!"
    }
    else {
        return "Unknown command";
    }
}

std::string Server::receiveMessage() {
    char buffer[BUFFER_SIZE]; // Create a buffer to hold the received message
    int n = recvfrom(serverSocket, buffer, static_cast<int>(sizeof(buffer) - 1), 0,
                     (sockaddr*)&client, &clientLen); // Receive a message from a client
    if (n == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom failed"); // Throw an exception if receiving the message fails
    }
    buffer[n] = '\0'; // Null-terminate the received message
    return std::string(buffer);
}

void Server::run() {
    while (true) {
        std::string msg = receiveMessage();
        std::string reply = processCommand(msg);
        send(reply);
        if (msg == "/exit") break;
        std::string logEntry = "Processed: " + msg + " -> " + reply;
        std::cout << logEntry << std::endl;
        logToFile(logEntry);
    }
}

void Server::logToFile(const std::string& entry) {
    std::ofstream log("server.log", std::ios::app); //append mode
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[64];
    sprintf_s(buf, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    if (log.is_open()) log << "[" << buf << "] " << entry << std::endl;
}

void Server::send(const std::string& message) {
    int sent = sendto(serverSocket, message.c_str(), static_cast<int>(message.size()), 0,
                      (sockaddr*)&client, clientLen); // Send a message to the client
    if (sent == SOCKET_ERROR) {
        throw std::runtime_error("sendto failed"); // Throw an exception if sending the message fails
    }
}