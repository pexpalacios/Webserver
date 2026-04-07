#include "../includes/PollServer.hpp"

//20260319 Alex: Init / Destroy
PollServer::PollServer() : _pollServer(), _servers() {};

PollServer::PollServer(std::vector<Server> servers) : _pollServer(), _servers(servers) {};

PollServer::~PollServer() {
	for (std::vector<int>::iterator it = _listenSockets.begin(); it != _listenSockets.end(); it++)
		close (*it);
};

// Methods

//20260319 Alex: builds a Poll of servers to listen and loads to a map<int, Server>
// same logic as Server::buildPollFdArray()
void PollServer::buildPollServerArray()
{
	// Let's cleanup just in case
	_pollFd.clear();
	_pollServer.clear();
	_listenSockets.clear();

	// Iterate through all Servers...
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		// Get all listenSockets and pair them with their Server
		std::vector<int> socketsVector = it->getListenSockets();

		// Let's iterate through all sockets
		for (size_t i = 0; i < socketsVector.size(); i++)
		{
			// Add the socket to a pullfd
			struct pollfd pfd;
			pfd.fd = socketsVector[i];
			pfd.events = POLLIN;
			pfd.revents = 0;

			// Add the pollfd to the vector that poll() reads
			_pollFd.push_back(pfd);

			// Map the fd to the addres of the current Server object
			_pollServer[socketsVector[i]] = &(*it);

			// Track this socket as a listenting socket for acceptt()
			_listenSockets.push_back(socketsVector[i]);

			std::cout << "Successfully registered listening FD " << socketsVector[i]
				<< " for Server: " << it->getStaticRoot() << std::endl;
		}
	}
}

//20260319 Alex: main poll() logic.
// poll() keeps track of diferent sockets ready for listen() and accept()
// If a socket is ready, the handler are called with their respective Server via map<socket, Server>
void PollServer::run()
{
	std::cout << "=== On PollServer::run()===" << std::endl;
	while (SignalHandler::running == 1)
	{
		int ret = poll(_pollFd.data(), _pollFd.size(), -1);
		if (ret < 0)
		{
			if (SignalHandler::running == 1)
				std::cerr << "Error (0.1): poll() failed." << std::endl;
			break;
		}

		// Check for events on each socket and use the handle of it Serve pair
		size_t j = 0;
		while (j < _pollFd.size())
		{
			int fd = _pollFd[j].fd;
			if (_pollFd[j].revents & POLLIN)
			{
				// Call to respective Server configuration for each socket
				Server *owner = _pollServer[fd];

				if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end()) 
					handleNewConnection(fd, *owner);
				else 
				{
					handleClientConnection(fd, *owner);
					_pollFd.erase(_pollFd.begin() + j); // I would remove this logic from here and movet to handleClientConnection()
					continue;
				}
			}
			// Some debugs 'cause why not
			else if (_pollFd[j].revents & POLLERR) std::cerr << "FD " << fd << " error!" << std::endl;
			else if (_pollFd[j].revents & POLLHUP) std::cerr << "FD " << fd << " hung up!" << std::endl;
			else if (_pollFd[j].revents & POLLNVAL) std::cerr << "FD " << fd << " is invalid!" << std::endl;
			++j;
		}
	}
}

// Private methods

// 20260319 Alex: Accept() new connection and added to the pollfd and map<socket, Server>
void PollServer::handleNewConnection(int listenSock, Server& server)
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

	// Add the pollfd to the vector that poll() reads
	_pollFd.push_back(newPollFd);

	// Map the fd to the addres of the current Server object
	_pollServer[clientSock] = &server;
}

// 20260319 Alex: recv() client connection, Server handler generates response
void PollServer::handleClientConnection(int clientSock, Server& server)
{
	std::string raw = server.recvRequest(clientSock);

	if (raw.empty())
	{
		close(clientSock);
		return;
	}

	std::cout << "Request received:\n" << raw << std::endl;

	Request request;
	if (!request.parse(raw))
	{
		std::cerr << "[ERROR] Invalid HTTP request" << std::endl;

		RequestHandler handler(server);
		Response responseObj = handler.handleBadRequest();

		std::string responseStr = responseObj.toString();
		send(clientSock, responseStr.c_str(), responseStr.size(), 0);

		std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
		std::cout << "-----------------------------------------------------\n" << std::endl;

		close(clientSock);
		return;
	}

	RequestHandler handler(server);
	Response responseObj;

	try
	{
		responseObj = handler.handleRequest(request, server);
	}
	catch (...)
	{
		std::cerr << "[ERROR] Internal Server Error" << std::endl;
		responseObj = handler.handleInternalServerError();
	}

	std::string response = responseObj.toString();
	send(clientSock, response.c_str(), response.size(), 0);

	std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
	std::cout << "-----------------------------------------------------\n" << std::endl;

	close(clientSock);
}