#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

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
        buffer[bytes_read] = '\0'; // Null-terminate the string
        printf("Received data: %s\n", buffer); // Print the received data

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
                write(client_socket, response, strlen(response)); // Send the response
            }
        }
    }
    close(client_socket); // Close the connection
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Creating the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
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

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue; // Handle the error, but continue accepting
        }
        handle_connection(client_socket);
    }

    close(server_socket);
    return 0;
}
