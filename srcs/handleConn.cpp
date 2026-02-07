#include "../includes/Server.hpp"
#include "../includes/handleConn.hpp"

// 20260207 Terto: Accept a new connection and add to pollfd array
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
void handleClientConnection(int clientSock, const std::string& staticRoot)
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

	std::string filePath = staticRoot + "/index.html";
	std::string body = readFile(filePath);
	if (body.empty())
		body = "<h1>404 Not Found</h1>";

	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
	response += "Connection: close\r\n\r\n";
	response += body;

	send(clientSock, response.c_str(), response.length(), 0);
	close(clientSock);
}
