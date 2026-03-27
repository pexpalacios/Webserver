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
	// Init / destructor
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
	std::map<int, ListenerKey>						_fdToListenerMap;
	std::vector<struct pollfd>						_pollFds;

	// Member function
	int createListenSocket(std::string& host, int port);
	bool loadAddrInfo(const std::string& host, int port, struct addrinfo **servinfo);

	// Cleanup
	void cleanup();
};

	// Build
// 	void setIpPortsPair();
// 	void setIpPortServerKeys();

// 	// Methods
// 	void handleNewConnection(int listenSock, Server& owner);
// 	void handleClientConnection(int listenSock, Server& owner);

// };

# endif