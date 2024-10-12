#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define PORT 9001
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

// Set the socket to non-blocking mode
int set_nonblocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }
    return 0;
}

// Parse HTTP request and check if it's a POST request
int parse_http_request(char* request, char* body) {
    if (strstr(request, "POST") != NULL) {
        char* content_length_str = strstr(request, "Content-Length:");
        if (content_length_str) {
            int content_length = atoi(content_length_str + 16); // Extract the content length value
            char* body_start = strstr(request, "\r\n\r\n") + 4;
            strncpy(body, body_start, content_length);
            return 1; // It's a POST request
        }
    }
    return 0; // Not a POST request
}

// Function to handle POST data (simple palindrome check)
void handle_post_data(char* body, int client_socket) {
    int is_palindrome = 1;
    int len = strlen(body);

    // Check if the body content is a palindrome
    for (int i = 0; i < len / 2; i++) {
        if (body[i] != body[len - i - 1]) {
            is_palindrome = 0;
            break;
        }
    }

    // Prepare and send HTTP response
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n\r\n%s",
        is_palindrome ? 7 : 8,
        is_palindrome ? "true" : "false");

    send(client_socket, response, strlen(response), 0);
}

// Main server loop with epoll
int main() {
    int server_socket, epoll_fd, num_fds;
    struct sockaddr_in server_address;
    struct epoll_event event, events[MAX_EVENTS];

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Set the socket to non-blocking
    set_nonblocking(server_socket);

    // Bind server socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Listen on the socket
    listen(server_socket, 5);
    printf("Server listening on port %d\n", PORT);

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Error creating epoll");
        close(server_socket);
        return 1;
    }

    // Add server socket to epoll
    event.data.fd = server_socket;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("Error adding server socket to epoll");
        close(server_socket);
        return 1;
    }

    // Variables for performance logging
    time_t start_time = time(NULL);
    int request_count = 0;

    // Main event loop
    while (1) {
        num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_fds; i++) {
            if (events[i].data.fd == server_socket) {
                // Accept new connection
                int client_socket = accept(server_socket, NULL, NULL);
                set_nonblocking(client_socket);

                // Add client socket to epoll
                event.data.fd = client_socket;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);

            } else {
                // Handle client data
                char buffer[BUFFER_SIZE];
                ssize_t bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';

                    // Check if it's a POST request
                    char body[BUFFER_SIZE];
                    if (parse_http_request(buffer, body)) {
                        handle_post_data(body, events[i].data.fd);
                    } else {
                        // If it's not a POST request, respond with 400 Bad Request
                        const char* bad_request_response =
                            "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nBad Request";
                        send(events[i].data.fd, bad_request_response, strlen(bad_request_response), 0);
                    }
                    request_count++;
                } else {
                    close(events[i].data.fd); // Close client connection
                }
            }
        }

        // Log performance every 10 seconds
        time_t current_time = time(NULL);
        if (current_time - start_time >= 10) {
            printf("Requests handled in last 10 seconds: %d\n", request_count);
            request_count = 0;
            start_time = current_time;
        }
    }

    close(server_socket);
    return 0;
}

