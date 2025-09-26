#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Signal handler for SIGCHLD
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Signal handler for shutdown
void sigint_handler(int sig) {
    printf("\nShutting down server gracefully...\n");
    exit(0);
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    // Read message from client
    bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Child process %d received: %s\n", getpid(), buffer);
        
        // Send response back to client
        const char* response = "Hello from C server";
        write(client_sock, response, strlen(response));
    }
    
    close(client_sock);
    printf("Child process %d finished handling client\n", getpid());
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pid_t pid;
    
    // Set up signal handlers
    signal(SIGCHLD, sigchld_handler);  
    signal(SIGINT, sigint_handler);   
    
    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Set socket option to reuse address
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket to address
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    // Start listening
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }
    
    printf("C Server listening on port %d...\n", PORT);
    
    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        
        if (client_sock < 0) {
            if (errno == EINTR) continue;  
            perror("Accept failed");
            continue;
        }
        
        printf("New client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Fork child process 
        pid = fork();
        if (pid == 0) {
            close(server_sock);  
            handle_client(client_sock);
            exit(0);
        } else if (pid > 0) {
            // Parent process
            close(client_sock);  
            printf("Forked child process %d to handle client\n", pid);
        } else {
            perror("Fork failed");
            close(client_sock);
        }
    }
    
    close(server_sock);
    return 0;
}