#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <liburing.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 64

// Function to handle HTTP response
void send_response(int client_socket, const char *response) {
    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}

// Function to process incoming data
void process_request(int client_socket, const char *data, ssize_t data_len) {
    // Find the end of headers (double CRLF)
    const char *body = data;
    const char *end_of_headers = strstr(data, "\r\n\r\n");
    if (end_of_headers) {
        // Move the pointer to the start of the body
        body = end_of_headers + 4; // Skip past the "\r\n\r\n"
        data_len = data + data_len - body; // Adjust the length to the body length
    } else {
        data_len = 0; // No body found
    }

    // Create a proper HTTP response
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "Received: %.*s", 
             data_len + 10, (int)data_len, body); // Include length of "Received: "

    send_response(client_socket, response);
}

// Function to handle incoming requests
void handle_request(struct io_uring *ring, struct io_uring_sqe *sqe, int client_socket) {
    char *buffer = malloc(BUFFER_SIZE); // Allocate buffer for receiving data
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(client_socket);
        return;
    }

    // Prepare to read data from the client socket
    struct iovec iov;
    iov.iov_base = buffer;
    iov.iov_len = BUFFER_SIZE;

    // Set up the read request
    io_uring_prep_recv(sqe, client_socket, buffer, BUFFER_SIZE, 0);
    sqe->user_data = (unsigned long)buffer; // Store buffer pointer for later use
}

// Function to set up the server
int setup_server() {
    int server_socket;
    struct sockaddr_in server_address;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Bind the socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(server_socket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Error listening on socket");
        close(server_socket);
        exit(1);
    }

    return server_socket;
}

int main() {
    struct io_uring ring;
    struct io_uring_sqe *sqe;
    int server_socket = setup_server();

    // Initialize io_uring
    if (io_uring_queue_init(QUEUE_SIZE, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connections
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Prepare to handle the request
        sqe = io_uring_get_sqe(&ring);
        handle_request(&ring, sqe, client_socket);

        // Submit the request
        io_uring_submit(&ring);

        // Wait for responses
        struct io_uring_cqe *cqe;
        while (io_uring_wait_cqe(&ring, &cqe) == 0) {
            unsigned long buffer_ptr = cqe->user_data; // Retrieve buffer pointer
            int socket_fd = client_socket; // Keep client socket

            if (cqe->res > 0) {
                // Process the received data
                char *data = (char *)buffer_ptr; // Use the buffer pointer directly
                process_request(socket_fd, data, cqe->res); // Send response
            } else {
                // Handle errors or close the connection
                close(socket_fd);
            }
            io_uring_cqe_seen(&ring, cqe);
            free((void *)buffer_ptr); // Free the allocated buffer
        }
    }

    // Clean up
    close(server_socket);
    io_uring_queue_exit(&ring);
    return 0;
}
