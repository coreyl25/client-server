#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080

// Signal handler
void sigchld_handler(int sig) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void handle_client(int client_sock) {
    char buffer[1024];
    
    // Read message from client
    int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Child process " << getpid() << " received: " << buffer << std::endl;
        
        // Send response back to client
        std::string response = "Hello from C++ server";
        write(client_sock, response.c_str(), response.length());
        std::cout << "Child process " << getpid() << " sent response" << std::endl;
    }
    
    close(client_sock);
    exit(0); 
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    
    // Install signal handler 
    signal(SIGCHLD, sigchld_handler);
    
    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(1);
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
        close(server_sock);
        exit(1);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_sock);
        exit(1);
    }
    
    // Listen for connections
    if (listen(server_sock, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_sock);
        exit(1);
    }
    
    std::cout << "C++ Server listening on port " << PORT << "..." << std::endl;
    
    // Accept connections in a loop
    while (true) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        
        if (client_sock < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        std::cout << "New client connected from " << inet_ntoa(client_addr.sin_addr) 
                  << ":" << ntohs(client_addr.sin_port) << std::endl;
        
        // Fork child process 
        pid_t child_pid = fork();
        
        if (child_pid == 0) {
            // Child process
            close(server_sock); 
            handle_client(client_sock);
        } else if (child_pid > 0) {
            // Parent process
            close(client_sock);
            std::cout << "Forked child process " << child_pid << " to handle client" << std::endl;
        } else {
            // Fork failed
            std::cerr << "Fork failed" << std::endl;
            close(client_sock);
        }
    }
    
    close(server_sock);
    return 0;
}