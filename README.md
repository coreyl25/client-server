# client-server
# Lab4: Client-Server Application Using Fork

This repository contains the implementation of a client-server application using the `fork()` system call in both C and C++. The server handles multiple clients concurrently by creating a child process for each client connection.

## Files Included

- `server.c` - C implementation of the fork-based server
- `client.c` - C implementation of the client
- `server.cpp` - C++ implementation of the fork-based server  
- `client.cpp` - C++ implementation of the client
- `Makefile` - Build automation file
- `README.md` - This instruction file

## Prerequisites

- Ubuntu or compatible Linux distribution
- GCC compiler for C programs
- G++ compiler for C++ programs
- Build essential tools

Install required tools:
``
sudo apt update && sudo apt install build-essential
``

## Compilation Instructions
Manual Compilation

For C implementation:
``
gcc server.c -o server
gcc client.c -o client
``

For C++ implementation:
``
g++ server.cpp -o server_cpp
g++ client.cpp -o client_cpp
``

## Running the Programs

### Testing the C Implementation
**Terminal - Run the C Client:**
   ``
   ./client
   ``

**Expected Output:**

Server Terminal:
``
C Server listening on port 8080...
New client connected from 127.0.0.1:xxxxx
Forked child process [PID] to handle client
Child process [PID] received: Hello Server
Child process [PID] sent response
``

Client Terminal:
``
Connected to server
Message sent: Hello Server
Server response: Hello from C server
``

### Testing the C++ Implementation

1. **Stop the C server (Ctrl+C)**

2. **Terminal - Start the C++ Server:**
   ``
   ./server_cpp
   ./client_cpp
   ``

**Expected Output:**

Server Terminal:
``
C++ Server listening on port 8080...
New client connected from 127.0.0.1:xxxxx
Forked child process [PID] to handle client
Child process [PID] received: Hello C++ Server
Child process [PID] sent response
``

Client Terminal:
``
Connected to server
Message sent: Hello C++ Server
Server response: Hello from C++ server
``
