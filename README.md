# Asynchronous-Web-Server

Introduction
This report outlines the implementation of a simple TCP server that checks whether a given input string is a palindrome. A palindrome is defined as a string that reads the same forwards and backwards. The server uses the HTTP protocol to communicate with clients, processing incoming requests and returning responses based on the evaluation of the input.

System Architecture
The server is built using the C programming language and employs socket programming to establish network communication. It listens for incoming TCP connections on a specified port and processes client requests in a loop. The architecture consists of the following components:

Socket Initialization: server_socket = socket(AF_INET, SOCK_STREAM, 0); This line creates a TCP socket using the socket() function. The parameters specify the address family (IPv4) and the socket type (stream). If the socket creation fails, an error message is printed, and the server exits.

Binding to an Address: bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)); The server binds the socket to an IP address and port number using the bind() function. This step is crucial as it allows the server to listen for connections directed to that specific port.

Listening for Connections: listen(server_socket, 5); The listen() function marks the socket as passive, enabling it to accept incoming connections. The server can queue a specified number of pending connections, in this case, up to 5.

Accepting Connections: int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len); The server enters an infinite loop, where it waits for client connections using the accept() function. Upon accepting a connection, a new socket dedicated to that client is created, allowing for simultaneous handling of multiple clients.

Request Handling
Upon receiving a connection, the server processes the client's HTTP request through the following steps:

Data Reception: int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1); This line reads the incoming data from the client socket into a buffer. The buffer is null-terminated to ensure proper string handling and prevent overflow.

Request Parsing: char *body = strstr(buffer, "\r\n\r\n"); The server parses the received HTTP request to extract the body content, which contains the string to be checked for palindrome properties. The strstr() function locates the end of the HTTP headers by searching for the double CRLF sequence.

Palindrome Checking: int palindrome = is_palindrome(body); The extracted body is passed to the is_palindrome() function. This function iterates through the string to determine if it reads the same forwards and backwards. It returns a boolean indicating whether the string is a palindrome.

Generating a Response: snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/plain\r\n\r\n%s", content_length, response_body); Based on the result of the palindrome check, the server constructs an HTTP response message. If the string is a palindrome, it responds with "The input is a palindrome." Otherwise, it responds with "The input is not a palindrome." The snprintf() function is used to safely format the response string.

Sending the Response: write(client_socket, response, strlen(response)); The server sends the generated HTTP response back to the client using the write() function, ensuring that the client receives feedback regarding their input.

Performance Analysis
The implementation utilizes epoll to efficiently handle thousands of concurrent connections by reacting only to active sockets. This non-blocking I/O model minimizes CPU usage, allowing the server to maintain responsiveness and manage high throughput under load.



OUTPUT
![Screenshot 2024-10-12 231153](https://github.com/user-attachments/assets/8a87d5ad-226e-45aa-a345-a7f327d58990)

