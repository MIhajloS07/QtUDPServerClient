#include "Client.h"
#include <stdexcept>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link against the Winsock library
#define BUFFER_SIZE 512
#define _WINSOCK_DEPRECATED_NO_WARNINGS // Disable deprecation warnings for Winsock functions


Client::Client(const std::string& serverIP, int port) {
    WSADATA wsa; // Declare a WSAData structure to hold information about the Windows Sockets implementation
    int result = WSAStartup(MAKEWORD(2, 2), &wsa); // Initialize Winsock
    if (result != 0) {
        throw std::runtime_error("WSAStartup failed"); // Throw an exception if Winsock initialization fails
    }

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("socket failed"); // Throw an exception if socket creation fails
    }

    memset(&server, 0, sizeof(server)); // initialize the server address structure to zero
    server.sin_family = AF_INET; // Set the address family to IPv4
    server.sin_port = htons(port); // Set the port number for the server

    std::wstring serverIPW(serverIP.begin(), serverIP.end()); // Convert the server IP address from std::string to std::wstring
    if (InetPton(AF_INET, serverIPW.c_str(), &server.sin_addr) <= 0) {
        closesocket(clientSocket); // Close the client socket if the server IP address is invalid
        WSACleanup(); // Clean up Winsock resources
        throw std::runtime_error("Invalid server IP address"); // Throw an exception if the server IP address is invalid
    }

    serverLen = sizeof(server); // Set the size of the server address structure
}

Client::~Client() {
    closesocket(clientSocket); // Close the client socket
    WSACleanup(); // Clean up Winsock resources
}

void Client::send(const std::string& message) {
    int sent = sendto(clientSocket, message.c_str(), static_cast<int>(message.size()), 0,
                      (sockaddr*)&server, serverLen); // Send a message to the server
    if (sent == SOCKET_ERROR) {
        throw std::runtime_error("sendto failed"); // Throw an exception if sending the message fails
    }
}

std::string Client::receiveMessage() {
    char buffer[BUFFER_SIZE]; // Create a buffer to hold the received message
    sockaddr_in fromAddr; // new structure for peer address
    int fromLen = sizeof(fromAddr);
    int n = recvfrom(clientSocket, buffer, static_cast<int>(sizeof(buffer) - 1), 0,
                     (sockaddr*)&fromAddr, &fromLen); // Receive a message from the server
    if (n == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom failed"); // Throw an exception if receiving the message fails
    }
    buffer[n] = '\0'; // Null-terminate the received message
    return std::string(buffer); // Return the received message as a string
}

bool Client::checkConnection(){
    char buffer[BUFFER_SIZE];

    const char *msg = "/ping";
    int sent = sendto(clientSocket, msg, strlen(msg), 0,
           (sockaddr*)&server, serverLen);
    if (sent == SOCKET_ERROR) return false;

    sockaddr_in fromAddr; // new structure for peer address
    int fromLen = sizeof(fromAddr);
    int recvLen = recvfrom(clientSocket, buffer, static_cast<int>(sizeof(buffer) - 1), 0,
                           (sockaddr*)&fromAddr, &fromLen);
    if (recvLen == SOCKET_ERROR) return false;

    buffer[recvLen] = '\0'; // Null-terminate the message
    return true; //if response sended, connection is succesed
}