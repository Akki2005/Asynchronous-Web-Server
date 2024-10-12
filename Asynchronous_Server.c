#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 9001
#define BUFFER_SIZE 256

// Function to handle each client connection
void* handle_client(void* arg) {
    int client_socket = *((int*)arg);
    free(arg); // Free the dynamically allocated memory
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            perror("Error receiving data");
            break; // Exit if there is an error or client disconnects
        }

        // Handle exit command
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }

        // Process the received data (e.g., check if it's a palindrome)
        int key = 1; // Assume it's a palindrome
        int n = strlen(buffer);
        for (int i = 0; i < n / 2; i++) {
            if (buffer[i] != buffer[n - i - 2]) { // Adjust for the newline character
                key = 0; // Not a palindrome
                break;
            }
        }

        // Send the result back to the client
        send(client_socket, &key, sizeof(key), 0);
    }

    close(client_socket); // Close the connection
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(server_socket);
        return 1;
    }

    listen(server_socket, 5);
    printf("Server listening on port %d\n", PORT);

    while (1) {
        int* client_socket = malloc(sizeof(int)); // Allocate memory for client socket
        *client_socket = accept(server_socket, NULL, NULL);
        if (*client_socket < 0) {
            perror("Error accepting connection");
            free(client_socket); // Free memory if accept fails
            continue;
        }

        // Create a new thread for the client connection
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
            perror("Error creating thread");
            close(*client_socket);
            free(client_socket);
        }
        pthread_detach(thread_id); // Detach the thread to handle cleanup automatically
    }

    close(server_socket); // Close the server socket
    return 0;
}
