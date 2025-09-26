#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in server_addr;
    std::string message = "Hello C++ Server";
    char buffer[1024];
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(1);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert IP address
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        close(sock);
        exit(1);
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        exit(1);
    }
    
    std::cout << "Connected to server" << std::endl;
    
    // Send message to server
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent: " << message << std::endl;
    
    // Read response from server
    int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }
    
    close(sock);
    return 0;
}