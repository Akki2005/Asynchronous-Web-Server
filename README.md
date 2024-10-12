# Asynchronous-Web-Server

Socket Programming:

Sockets are endpoints for sending and receiving data across a network.
The server uses TCP sockets to listen for incoming connections on a specified port (8080).
Asynchronous I/O:

Utilizes liburing, a library for efficient asynchronous I/O operations.
Allows the server to handle multiple client connections without blocking, improving performance.
HTTP Protocol:

The server implements a simple HTTP response structure, recognizing HTTP requests and sending back formatted responses.
The response includes status codes (e.g., 200 OK), content type, and content length.
Memory Management:

Dynamic memory allocation (malloc()) is used for creating a buffer to store incoming request data.
It's essential to manage memory properly to avoid leaks, especially in a server environment where requests can be numerous.
String Manipulation:

The code uses functions like strstr() to locate the end of HTTP headers.
Processes incoming data by checking for the end of headers and adjusting the data length accordingly.
Error Handling:

The server checks for errors at each step, such as socket creation, binding, and listening, ensuring robust operation.
Proper error handling prevents crashes and provides meaningful feedback.
HTTP Response Construction:

Constructs the HTTP response based on the received data, indicating what was processed (in this case, checking if the input is a palindrome).
The response format follows the HTTP specification, ensuring clients can interpret it correctly.
Client Handling:

The server accepts client connections and processes requests in an event-driven manner, allowing for high concurrency.
Each client request is handled independently, ensuring that multiple clients can interact with the server simultaneously.

Key Keywords
socket: Creates a socket for communication.
bind: Associates the socket with a specific address and port.
listen: Listens for incoming connections on the socket.
accept: Accepts a connection from a client.
send: Sends data to a connected client.
close: Closes the socket connection.
malloc: Allocates memory for data storage.
strstr: Searches for a substring within a string.
snprintf: Formats a string for the HTTP response.
io_uring: Manages asynchronous I/O operations efficiently.
OUTPUT :
Server :
![Screenshot 2024-10-12 105145](https://github.com/user-attachments/assets/9fe25ba0-769f-4696-9ec4-bb3529f05f37)
Client :
![Screenshot 2024-10-12 105209](https://github.com/user-attachments/assets/86843edc-725e-496b-b1df-1dcd3e870b80)

