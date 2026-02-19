#include "../includes/Server.hpp"
#include "../includes/Signal.hpp"


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
	std::cout << "Waiting for connections..." << std::endl;
	return fds;
}

//20260207 Terto: Accept a new connection and add to pollfd array
// main -> server.run() -> handleNewConnection() -> accept() + push_back a fds
// Accepts a new incoming connection and adds it to the pollfd array for monitoring
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

// 20260207 Terto: Handle a connected client socket
// main -> server.run() -> handleClientConnection() -> recv() + server.readFile() + send() + close()
// Receives an HTTP request from a client, responds with index.html or 404, then closes the connection
void Server::handleClientConnection(int clientSock, Server& server)
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

	std::string filePath = server.staticRoot + "/" + server.indexFile;
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


//20260210 Terto: Inicia el bucle principal del servidor, usando poll() para manejar múltiples conexiones
// main -> server.run()
void Server::run() 
{
	// Agrega los sockets de escucha al array de pollfd
	std::vector<struct pollfd> fds = buildPollFdArray();

	// main loop with SIGINT (Ctrl+C) to stop the server
	while (SignalHandler::running == 1)
	{
		int ret = poll(&fds[0], fds.size(), -1);
		if (ret < 0) {
			std::cerr << "Error (0.1): poll() failed." << std::endl;
			break;
		}

		// Check for events on each socket
		size_t j = 0;
		while (j < fds.size()) {
			if (fds[j].revents & POLLIN) 
			{
				if (std::find(listenSockets.begin(), listenSockets.end(), fds[j].fd) != listenSockets.end()) 
				{
					handleNewConnection(fds[j].fd, fds);
				} 
				else 
				{
					handleClientConnection(fds[j].fd, *this);
					fds.erase(fds.begin() + j);
					continue;
				}
			}
			++j;
		}
	}
}
