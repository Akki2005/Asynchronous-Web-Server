# Asynchronous-Web-Server

Introduction
This report outlines the implementation of a simple TCP server that checks whether a given input string is a palindrome. A palindrome is defined as a string that reads the same forwards and backwards. The server uses the HTTP protocol to communicate with clients, processing incoming requests and returning responses based on the evaluation of the input.

System Architecture
The server is built using the C programming language and leverages socket programming to establish communication over the network. It listens for incoming TCP connections on a specified port and processes client requests in a loop. The server's architecture consists of the following components:

Socket Initialization: The server creates a TCP socket using the socket() function. It specifies the address family (IPv4) and the socket type (stream).

Binding to an Address: The server binds the socket to an IP address and port number using the bind() function. This allows the server to listen for connections directed to that specific port.

Listening for Connections: The listen() function is called to mark the socket as passive, allowing it to accept incoming connections. The server can queue a specified number of pending connections.

Accepting Connections: The server enters an infinite loop, where it waits for client connections using the accept() function. Upon accepting a connection, it creates a new socket dedicated to that client.

Request Handling
Upon receiving a connection, the server reads the client's HTTP request. The handling process involves the following steps:

Data Reception: The server reads the incoming data from the client socket into a buffer, ensuring the buffer is null-terminated for proper string handling.

Request Parsing: The server parses the received HTTP request to extract the body content, which contains the string to be checked for palindrome properties.

Palindrome Checking: The extracted body is passed to the is_palindrome() function, which iterates through the string to determine if it reads the same forwards and backwards. This function returns a boolean indicating whether the string is a palindrome.

Generating a Response: Based on the result of the palindrome check, the server constructs an HTTP response message. If the string is a palindrome, it responds with "The input is a palindrome." Otherwise, it responds with "The input is not a palindrome."

Sending the Response: The server sends the generated HTTP response back to the client using the write() function, ensuring that the client receives feedback regarding their input.

Conclusion
This TCP server demonstrates a straightforward application of socket programming and HTTP request handling in C. It efficiently processes client connections and checks for palindrome strings, providing immediate feedback through HTTP responses. The design can serve as a foundation for more complex applications involving string processing and network communication.
OUTPUT
![Screenshot 2024-10-12 231153](https://github.com/user-attachments/assets/8a87d5ad-226e-45aa-a345-a7f327d58990)

