#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = "Hello Server";
    char response[BUFFER_SIZE];
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert IP address
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        exit(1);
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(1);
    }
    
    printf("Connected to server\n");
    
    // Send message to server
    send(sock, buffer, strlen(buffer), 0);
    printf("Message sent: %s\n", buffer);
    
    // Read response from server
    int bytes_read = read(sock, response, sizeof(response) - 1);
    if (bytes_read > 0) {
        response[bytes_read] = '\0';
        printf("Server response: %s\n", response);
    }
    
    close(sock);
    return 0;
}