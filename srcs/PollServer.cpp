#include "../includes/PollServer.hpp"

//20260319 Alex: Init / Destroy
PollServer::PollServer() : _servers() {};

PollServer::PollServer(std::vector<Server> servers) : _servers(servers) {};

PollServer::~PollServer()
{
	cleanup();
};

// Public Methods

//20260327 Alex: Reads from all servers, populates all structs and builds _pollFds;
void PollServer::buildPollServerArray()
{
	_pollFds.clear();
	_listeners.clear();
	_listenerServers.clear();

	// Build _listener map
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		Server* server = &_servers[i];
		const std::vector<int>& ports = server->getPorts();

		for (size_t j = 0; j < ports.size(); ++j)
		{
			ListenerKey key;
			key._host = server->getHost();
			key._port = ports[j];

			_listeners[key].push_back(server);
		}
	}

	for (std::map<ListenerKey, std::vector<Server*> >::iterator it = _listeners.begin();
		it != _listeners.end(); ++it)
	{
		const ListenerKey& key = it->first;
		std::vector<Server*>& servers = it->second;

		int fd = createListenSocket(key._host, key._port);
		if (fd < 0)
			throw std::runtime_error("Failed to create listen socket");

		struct pollfd pfd;
		pfd.fd = fd;
		pfd.events = POLLIN; 
		pfd.revents = 0;
		_pollFds.push_back(pfd);

		_listenerServers[fd] = servers; 
	}
}


// Private methods

// 20260327 Alex: Create a listen socket with fcntl(O_NONBLOCK) and setsockopt(SO_REUSADDR) flags
int PollServer::createListenSocket(const std::string& host, int ip)
{
		// Let's init that addrinfo struct
		struct addrinfo *servinfo = NULL;
		if (!loadAddrInfo(host, ip, &servinfo))
			return (-1);

		// Same logic as old, only using addrinfo servinfo!
		int sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
		// int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1) {
			freeaddrinfo(servinfo);
			std::cerr << "Error (0.1): creating socket" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		{
			freeaddrinfo(servinfo);
			close(sock);
			throw std::runtime_error("Failed (0.2): set non-blocking");
		}

		int opt = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			freeaddrinfo(servinfo);
			close(sock);
			throw std::runtime_error("Failed (0.3): setsockopt");
		}

		if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{
			freeaddrinfo(servinfo);
			close(sock);
			throw std::runtime_error("Failed (0.4): bind");
		}

		if (listen(sock, 100) == -1)
		{	
			freeaddrinfo(servinfo);
			close(sock);
			throw std::runtime_error("Failed (0.5): listen");
		}
		freeaddrinfo(servinfo);
		return (sock);
}

// 20260327 Alex: helper to load gettadrrinfo struct
bool PollServer::loadAddrInfo(const std::string& host, int port, struct addrinfo **servinfo)
{
		int status;
		std::ostringstream port_number;
		port_number << port;
		struct addrinfo hints = {};

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if ((status = getaddrinfo(host.c_str(), port_number.str(). c_str(), &hints, servinfo)) != 0)
		{
			std::cerr << "Error (0.0): getaddrinfo()" << gai_strerror(status) << std::endl;
			return (false);
		}
	return (true);
}

// 20260330 Alex: main run() loop
void PollServer::run(){
	std::cout << "=== On PollServer::run()===" << std::endl;

	while (SignalHandler::running == 1)
	{
		int ret = poll(_pollFds.data(), _pollFds.size(), -1);
		if (ret < 0)
		{
			if (SignalHandler::running == 1)
				std::cerr << "Error (0.1): poll () failed." << std::endl;
			break;
		}

		// Check for events on each socket and use the handler of it's Server pair
		for (size_t j = 0; j < _pollFds.size(); ++j)
		{
			int fd = _pollFds[j].fd;
			if (_pollFds[j].revents & POLLIN)
			{
				if (_listenerServers.count(fd))
				handleNewConnection(fd);
				else
				handleClientConnection(fd);
			}
			else if (_pollFds[j].revents & POLLERR) std::cerr << "FD " << fd << " error!" << std::endl;
			else if (_pollFds[j].revents & POLLHUP) std::cerr << "FD " << fd << " hung up!" << std::endl;
			else if (_pollFds[j].revents & POLLNVAL) std::cerr << "FD " << fd << " is invalid!" << std::endl;
		}
	}
}

void PollServer::handleNewConnection(int listenSock)
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
	_pollFds.push_back(newPollFd);

	// Copies all the std::vector<Server*> that listen to that socket to the client socket
	_clientCandidates[clientSock] = _listenerServers[listenSock];
}

// 20260319 Alex: recv() client connection, Server handler generates response
void PollServer::handleClientConnection(int clientSock)
{
	// First, let's check if there's a clientcandidate with that socket
	std::map<int, std::vector<Server*> >::iterator it = _clientCandidates.find(clientSock);
	if (it == _clientCandidates.end())
	{
		std::cerr << "Unknown client fd " << clientSock << std::endl;
		removePollFd(clientSock);
		close(clientSock);
		return;
	}

	// Let's get the Server vector from our clientSocket
	std::vector<Server*>& candidates = it->second;
	if (candidates.empty())
	{
		std::cerr << "No server candidates for client fd " << clientSock << std::endl;
		removePollFd(clientSock);
		close(clientSock);
		_clientCandidates.erase(clientSock);
		return;
	}
	//Lets set a default server in case something went wrong
	Server *chosenServer = candidates[0];

	// Let's receive the raw request
	std::string raw = recvRequest(clientSock);
	if (raw.empty())
	{
		removePollFd(clientSock);
		close(clientSock);
		_clientCandidates.erase(clientSock);
		return;
	}
	
	// // Let's parse that request!
	Request request;
	if (!request.parse(raw))
	{
		std::cerr << "[ERROR] Invalid HTTP request" << std::endl;

		// All the handlers will work for this case?
		RequestHandler handler(*chosenServer);
		Response responseObj = handler.handleBadRequest();

		std::string responseStr = responseObj.toString();
		send(clientSock, responseStr.c_str(), responseStr.size(), 0);

		std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
		std::cout << "-----------------------------------------------------\n" << std::endl;

		close(clientSock);
		return;
	}

	// // Finally, let's select our server based on host header!!!!
	std::string host = request.getHeader("Host");
	std::string hostHeader = host;
	size_t colon = host.find(':');
	if (colon != std::string::npos)
		hostHeader = host.substr(0,colon);
	std::cout << "===Host Header for request===" << std::endl;
	std::cout << hostHeader << std::endl;
	// This will work only if
	// We parse host as an string and not exactly as an Ipv4
	// We set a /etc/hosts/ key
	// As we can do any of that, I'll try to compare to an custom header in the future
	std::cout << "=== Candidates size ===" << std::endl;
	std::cout << candidates.size() << std::endl;
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		std::cout << "===Host Header Candidates===" << std::endl;
		std::cout << candidates[i]->getServerName() << std::endl;
		if (candidates[i]->getServerName() == hostHeader)
		{
			std::cout << "=== Candidate choosen ===" << std::endl;
			chosenServer = candidates[i];
			break;
		}
	}

	RequestHandler handler(*chosenServer);
	Response responseObj;

	try
	{
		responseObj = handler.handleRequest(request);
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

	removePollFd(clientSock);
	close(clientSock);
	_clientCandidates.erase(clientSock);
}

// 20260330 Alex: same as Server::recvRequest()
std::string PollServer::recvRequest(int clientSock)
{
	char buffer[4096];
	std::string raw;

	size_t contentLength = 0;
	bool headersParsed = false;
	size_t headerEnd = std::string::npos;

	while (true)
	{
		ssize_t bytesRead = recv(clientSock, buffer, sizeof(buffer), 0);

		if (bytesRead > 0)
		{
			raw.append(buffer, bytesRead);

			if (!headersParsed)
			{
				headerEnd = raw.find("\r\n\r\n");

				if (headerEnd != std::string::npos)
				{
					headersParsed = true;

					size_t pos = raw.find("Content-Length:");
					if (pos != std::string::npos)
					{
						size_t start = pos + 15;
						while (raw[start] == ' ')
							start++;

						size_t end = raw.find("\r\n", start);
						contentLength = std::atoi(raw.substr(start, end - start).c_str());
					}

					if (contentLength == 0)
						break;
				}
			}

			if (headersParsed)
			{
				size_t bodyStart = headerEnd + 4;

				if (raw.size() >= bodyStart + contentLength)
					break;
			}
		}
		else if (bytesRead == 0)
		{
			break;
		}
		else
		{
			std::cerr << "recv() failed: " << std::endl;
			return "";
		}
	}

	return raw;
}


// 20260330 Alex: remove an specific fd from the poll
void PollServer::removePollFd(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			return;
		}
	}
}

// 20260327 Alex: close all fds and clear structs
void PollServer::cleanup()
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		int fd = _pollFds[i].fd;
		if (fd >= 0)
			close(fd); // close the socket
	}

	// Clear your vectors/maps to avoid dangling pointers
	_pollFds.clear();
	_listeners.clear();
	_listenerServers.clear();
	_clientCandidates.clear();
}

// Operators
// 20260327 Alex: operator < for mapping ListenerKey
bool PollServer::ListenerKey::operator<(const ListenerKey& other) const
{
		if (_host != other._host)
			return _host < other._host;
		return _port < other._port;
}


//// ======= OLD CODE FOR REFERENCE =======

//20260319 Alex: builds a Poll of se_virtualHostsrvers to listen and loads to a map<int, Server>
// // same logic as Server::buildPollFdArray()
// void PollServer::buildPollServerArray()
// {
// 	// Let's cleanup just in case
// 	_pollFd.clear();
// 	_pollServer.clear();
// 	_listenSockets.clear();

// 	// Iterate through all Servers...
// 	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
// 	{
// 		// Get all listenSockets and pair them with their Server
// 		std::vector<int> socketsVector = it->getListenSockets();

// 		// Let's iterate through all sockets
// 		for (size_t i = 0; i < socketsVector.size(); i++)
// 		{
// 			// Add the socket to a pullfd
// 			struct pollfd pfd;
// 			pfd.fd = socketsVector[i];
// 			pfd.events = POLLIN;
// 			pfd.revents = 0;

// 			// Add the pollfd to the vector that poll() reads
// 			_pollFd.push_back(pfd);

// 			// Map the fd to the addres of the current Server object
// 			_pollServer[socketsVector[i]] = &(*it);

// 			// Track this socket as a listenting socket for acceptt()
// 			_listenSockets.push_back(socketsVector[i]);

// 			std::cout << "Successfully registered listening FD " << socketsVector[i]
// 				<< " for Server: " << it->getStaticRoot() << std::endl;
// 		}
// 	}
// }

//20260319 Alex: main poll() logic.
// poll() keeps track of diferent sockets ready for listen() and accept()
// If a socket is ready, the handler are called with their respective Server via map<socket, Server>
// void PollServer::run()
// {
// 	std::cout << "=== On PollServer::run()===" << std::endl;
// 	while (SignalHandler::running == 1)
// 	{
// 		int ret = poll(_pollFd.data(), _pollFd.size(), -1);
// 		if (ret < 0)
// 		{
// 			if (SignalHandler::running == 1)
// 				std::cerr << "Error (0.1): poll() failed." << std::endl;
// 			break;
// 		}

// 		// Check for events on each socket and use the handle of it Serve pair
// 		size_t j = 0;
// 		while (j < _pollFd.size())
// 		{
// 			int fd = _pollFd[j].fd;
// 			if (_pollFd[j].revents & POLLIN)
// 			{
// 				// Call to respective Server configuration for each socket
// 				Server *owner = _pollServer[fd];

// 				if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end()) 
// 					handleNewConnection(fd, *owner);
// 				else 
// 				{
// 					handleClientConnection(fd, *owner);
// 					_pollFd.erase(_pollFd.begin() + j); // I would remove this logic from here and movet to handleClientConnection()
// 					continue;
// 				}
// 			}
// 			// Some debugs 'cause why not
// 			else if (_pollFd[j].revents & POLLERR) std::cerr << "FD " << fd << " error!" << std::endl;
// 			else if (_pollFd[j].revents & POLLHUP) std::cerr << "FD " << fd << " hung up!" << std::endl;
// 			else if (_pollFd[j].revents & POLLNVAL) std::cerr << "FD " << fd << " is invalid!" << std::endl;
// 			++j;
// 		}
// 	}
// }

// Private methods

// // 20260319 Alex: Accept() new connection and added to the pollfd and map<socket, Server>
// void PollServer::handleNewConnection(int listenSock, Server& server)
// {
// 	struct sockaddr_in clientAddr;
// 	socklen_t clientLen = sizeof(clientAddr);

// 	int clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientLen);
// 	if (clientSock < 0)
// 	{
// 		std::cerr << "accept() failed." << std::endl;
// 		return;
// 	}
// 	std::cout << "Client connected. FD = " << clientSock << std::endl;

// 	// Set client socket to non-blocking
// 	fcntl(clientSock, F_SETFL, O_NONBLOCK);

// 	// Add client socket to pollfd array
// 	struct pollfd newPollFd;
// 	newPollFd.fd = clientSock;
// 	newPollFd.events = POLLIN;
// 	newPollFd.revents = 0;

// 	// Add the pollfd to the vector that poll() reads
// 	_pollFd.push_back(newPollFd);

// 	// Map the fd to the addres of the current Server object
// 	_pollServer[clientSock] = &server;
// }

// // 20260319 Alex: recv() client connection, Server handler generates response
// void PollServer::handleClientConnection(int clientSock, Server& server)
// {
// 	std::string raw = server.recvRequest(clientSock);

// 	if (raw.empty())
// 	{
// 		close(clientSock);
// 		return;
// 	}

// 	Request request;
// 	if (!request.parse(raw))
// 	{
// 		std::cerr << "[ERROR] Invalid HTTP request" << std::endl;

// 		RequestHandler handler(server);
// 		Response responseObj = handler.handleBadRequest();

// 		std::string responseStr = responseObj.toString();
// 		send(clientSock, responseStr.c_str(), responseStr.size(), 0);

// 		std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
// 		std::cout << "-----------------------------------------------------\n" << std::endl;

// 		close(clientSock);
// 		return;
// 	}

// 	RequestHandler handler(server);
// 	Response responseObj;

// 	try
// 	{
// 		responseObj = handler.handleRequest(request);
// 	}
// 	catch (...)
// 	{
// 		std::cerr << "[ERROR] Internal Server Error" << std::endl;
// 		responseObj = handler.handleInternalServerError();
// 	}

// 	std::string response = responseObj.toString();
// 	send(clientSock, response.c_str(), response.size(), 0);

// 	std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
// 	std::cout << "-----------------------------------------------------\n" << std::endl;

// 	close(clientSock);
// }

