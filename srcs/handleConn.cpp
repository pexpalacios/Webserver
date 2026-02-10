#include "../includes/handleConn.hpp"

//20260207 Terto: Accept a new connection and add to pollfd array
// main -> server.run() -> handleNewConnection() -> accept() + push_back a fds
// Accepts a new incoming connection and adds it to the pollfd array for monitoring
void handleNewConnection(int listenSock, std::vector<struct pollfd>& fds)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientSock < 0)
	{
		std::cerr << "accept() failed." << std::endl;
		return;
	}
	std::cout << "Client connected. FD = " << clientSock << std::endl;

	// Set client socket to non-blocking
	fcntl(clientSock, F_SETFL, O_NONBLOCK);

	// Add client socket to pollfd array
	struct pollfd newPollFd;
	newPollFd.fd = clientSock;
	newPollFd.events = POLLIN;
	newPollFd.revents = 0;
	fds.push_back(newPollFd);
}

// 20260207 Terto: Handle a connected client socket
// main -> server.run() -> handleClientConnection() -> recv() + server.readFile() + send() + close()
// Receives an HTTP request from a client, responds with index.html or 404, then closes the connection
void handleClientConnection(int clientSock, Server& server)
{
	char buffer[1024];
	int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		std::cerr << "recv() failed or client disconnected. FD " << clientSock << std::endl;
		close(clientSock);
		return;
	}
	buffer[bytesRead] = '\0';

	std::cout << "Request received from FD " << clientSock << ":\n" << buffer << std::endl;

	std::string filePath = server.staticRoot + "/index.html";
	std::string body = server.readFile(filePath);
	if (body.empty())
		body = "<h1>404 Not Found</h1>";

	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	std::ostringstream oss;
	oss << body.length();
	response += "Content-Length: " + oss.str() + "\r\n";
	response += "Connection: close\r\n\r\n";
	response += body;

	send(clientSock, response.c_str(), response.length(), 0);
	close(clientSock);
}
