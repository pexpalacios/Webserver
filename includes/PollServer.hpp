#ifndef POLLSERVER_HPP
	#define POLLSERVER_HPP

#include "library.hpp"
#include "parse/ConfigParser.hpp"
#include "parse/LocationConfig.hpp"
#include "parse/ServerConfig.hpp"
#include "http/RequestHandler.hpp"
#include "Server.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"

#include "Signal.hpp"

class PollServer
{
public:
	// Init / destructor
	PollServer();
	PollServer(std::vector<Server>);
	~PollServer();

	// Methods
	void buildPollServerArray();
	void run();
	//void setDefaultServer();

private:
	std::map<int, Server*> _pollServer; // Store a vector pair of socketFD and Server
	std::vector<struct pollfd> _pollFd;
	std::vector<int> _listenSockets;
	std::vector<Server> _servers;

	// Methods
	void handleNewConnection(int listenSock, Server& owner);
	void handleClientConnection(int listenSock, Server& owner);

};

# endif