#include "../includes/Server.hpp"
#include "../includes/Signal.hpp"
#include "../includes/http/RequestHandler.hpp"

//20260210 Terto: read file content and return as string
// main -> server.run() -> handleClientConnection() -> server.readFile()
std::string Server::readFile(const std::string& path) 
{
	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		std::cerr << "Error (0.1): Failed to open file: " << path << std::endl;
		return "";
	}

	std::string content;
	std::string line;
	while (std::getline(file, line))
		content += line + "\n";

	file.close();
	return content;
}


// 20260210 Terto: Build and return pollfd array from listenSockets
// main -> server.run() -> buildPollFdArray() -> pollfd array with listenSockets
std::vector<struct pollfd> Server::buildPollFdArray()
{
	std::vector<struct pollfd> fds;
	size_t i = 0;
	while (i < listenSockets.size()) 
	{
		struct pollfd pfd;
		pfd.fd = listenSockets[i];
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
		++i;
	} 
	std::cout << "Waiting for connections..." << std::endl << std::endl;
	std::cout << "-----------------------------------------------------" << std::endl << std::endl;
	return fds;
}


//20260207 Terto: Accept a new connection and add to pollfd array
// main -> server.run() -> handleNewConnection() -> accept() + push_back a fds
void Server::handleNewConnection(int listenSock, std::vector<struct pollfd>& fds)
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
