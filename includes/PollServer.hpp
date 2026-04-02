#ifndef POLLSERVER_HPP
# define POLLSERVER_HPP

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
	// Init / destructor, need to create OCF
	PollServer();
	PollServer(std::vector<Server>);
	~PollServer();

	// Methods
	void buildPollServerArray();
	void run();

private:
	// Members struct
	struct ListenerKey
	{
		std::string				_host;
		int						_port;

		bool operator<(const ListenerKey& other) const;
	};

	// Members
	std::vector<Server>								_servers;
	std::map<ListenerKey, std::vector<Server*> >	_listeners;
	// std::map<int, ListenerKey>						_fdToListenerMap;
	std::map<int, std::vector<Server*> >			_listenerServers;
	std::map<int, std::vector<Server*> >			_clientCandidates;
	std::vector<struct pollfd>						_pollFds;

	// Member function
	int createListenSocket(const std::string& host, int port);
	bool loadAddrInfo(const std::string& host, int port, struct addrinfo **servinfo);
	void handleNewConnection(int listenSock);
	void handleClientConnection(int clientSock);
	std::string recvRequest(int clientSock);
	// Cleanup
	void cleanup();
	void removePollFd(int fd);
};

# endif