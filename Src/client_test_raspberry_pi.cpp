#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8443       // Port to connect to
#define SERVER_IP "192.168.188.52"   // Server IP (localhost)
#define BUFFER_SIZE 1024        // Buffer size for messages

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Define the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(client_socket);
        return 1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection to the server failed");
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to the server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    // Send and receive messages in a loop
    while (true) {
        std::cout << "Enter a message (or type 'exit' to quit): ";
        std::string message;
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // Send the message to the server
        if (send(client_socket, message.c_str(), message.size(), 0) == -1) {
            perror("Failed to send message");
            break;
        }

        // Receive the server's response
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("Failed to receive message");
            break;
        }

        buffer[bytes_received] = '\0';  // Null-terminate the received message
        std::cout << "Server echoed: " << buffer << std::endl;
    }

    // Close the socket
    close(client_socket);
    std::cout << "Disconnected from the server." << std::endl;

    return 0;
}
