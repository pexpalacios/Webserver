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
	std::vector<Server>								_servers; // Array of Servers after parsing
	std::map<ListenerKey, std::vector<Server*> >	_listeners; // A map of the ListenerKey(Host:Port) and the array of vector that listen to that key
	std::map<int, std::vector<Server*> >			_listenerServers; // Map of listen sockets and the servers associated to each one
	std::map<int, std::vector<Server*> >			_clientCandidates; // Map of client sockets and the Servers associated to each one
	std::vector<struct pollfd>						_pollFds; // Fds to poll()

	// 20260411 Terto: Buffer parcial por cliente para recv
	std::map<int, std::string>						_partialRequests; // Buffer parcial por cliente para reconstruir requests fragmentadas

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