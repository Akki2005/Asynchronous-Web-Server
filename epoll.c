#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

// Function to check if a string is a palindrome
int is_palindrome(const char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        if (str[i] != str[len - i - 1]) {
            return 0; // Not a palindrome
        }
    }
    return 1; // Is a palindrome
}

void handle_connection(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; 
        printf("Received data: %s\n", buffer); 

        // Simple parsing logic to extract the body
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4; // Move past the headers
            
            // Check if the body has data
            if (strlen(body) > 0) {
                int palindrome = is_palindrome(body);

                // Create the response based on whether it's a palindrome
                char response[BUFFER_SIZE];
                if (palindrome) {
                    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: 29\r\nContent-Type: text/plain\r\n\r\nThe input is a palindrome.");
                } else {
                    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: 34\r\nContent-Type: text/plain\r\n\r\nThe input is not a palindrome.");
                }
                write(client_socket, response, strlen(response)); 
            }
        }
    }
    close(client_socket); // Close the connection
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;

    // Creating the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Create an epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        // Wait for events
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_socket) {
                // Accept a new connection
                int client_socket = accept(server_socket, NULL, NULL);
                if (client_socket < 0) {
                    perror("Accept failed");
                    continue;
                }
                // Add the new client socket to the epoll instance
                event.events = EPOLLIN;
                event.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("epoll_ctl failed");
                    close(client_socket);
                    continue;
                }
            } else {
                // Handle data from a client socket
                handle_connection(events[i].data.fd);
                // Remove the client socket from epoll
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }

    close(server_socket);
    return 0;
}

