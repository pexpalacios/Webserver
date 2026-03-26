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

private:
	struct IpPortServerKey
	{
		std::string host;
		int port;
		std::vector<Server> _keyServers;

		bool operator<(const IpPortServerKey &other) const;
	};
	std::map<std::pair<std::string, int>, std::vector<Server*> > _virtualHosts;
	std::map<int, Server*> _pollServer; // Store a vector pair of socketFD and Server
	std::vector<struct pollfd> _pollFd;
	std::vector<int> _listenSockets;
	std::vector<Server> _servers;
	std::set<IpPortServerKey> IpPortsServerKeys;

	// Build
	void setIpPortsPair();
	void setIpPortServerKeys();

	// Methods
	void handleNewConnection(int listenSock, Server& owner);
	void handleClientConnection(int listenSock, Server& owner);

};

# endif