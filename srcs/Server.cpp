#include "../includes/Server.hpp"
#include "../includes/Signal.hpp"

Server::Server() {}

//20260319 Alex: moved close() from closeSockets() to ~Server()
// This is actually wrong, remove
Server::~Server() {}

const std::string& Server::getServerName() const
{return serverName;};

const std::string& Server::getHost() const
{return host;};

const std::vector<int>& Server::getPorts() const
{return ports;};

const std::string& Server::getStaticRoot() const
{return staticRoot;}

const std::string& Server::getIndexFile() const
{return indexFile;}

const std::map<int, std::string>& Server::getErrorPages() const
{return errorPages;}

const std::vector<LocationConfig>& Server::getLocations() const
{return locations;}

const std::vector<int>& Server::getListenSockets() const
{return listenSockets;}

void Server::configureMaxBodySize(long long size)
{_maxBodySize = size;}

// 20260322 Alex: load struct addrinfo
bool Server::loadAddrinfo(const std::string& ip, int port, struct addrinfo **servinfo)
{
		int status;
		std::ostringstream port_number;
		port_number << port;
		struct addrinfo hints = {};

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if ((status = getaddrinfo(ip.c_str(), port_number.str(). c_str(), &hints, servinfo)) != 0)
		{
			std::cerr << "Error (0.0): getaddrinfo()" << gai_strerror(status) << std::endl;
			return (false);
		}
	return (true);
}

//20260212 Terto: configure server socket to listen on specific IP and port
//20260311 Alex: modified to iterate through vector<int> ports
//20260322 Alex: modified to use struct addrinfo. Clean up is pretty ugly if you ask me, i'll work on RAII for cleaner code
// main -> server.configureServer() -> server.listenOn()
void Server::listenOn(const std::string& ip, std::vector<int> ports) 
{
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++)
	{
		// Let's init that addrinfo struct
		struct addrinfo *servinfo = NULL;
		if (!loadAddrinfo(ip, *it, &servinfo))
			continue;

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

		listenSockets.push_back(sock);
		std::cout << "Listening on " << ip << ":" << *it << std::endl;
		freeaddrinfo(servinfo);
	}
}

//20250322 Alex: set ip addres
// main -> server.configureServer -> server.setHost
void	Server::setHost(const std::string& host)
{
	this->host = host;
}

//20250322 Alex: set vector<int> ports
// main -> server.configureServer -> server.setHost
void	Server::setPorts(const std::vector<int>& ports)
{
	this->ports = ports;
}

//20260212 Terto: define static root folder for serving static files (www/index.html)
// main -> server.configureServer() -> server.setStaticRoot()
void Server::setStaticRoot(const std::string& root, const std::string& index)
{
	staticRoot = root;
	indexFile = index;

	std::cout << "Static root set to: " << staticRoot << std::endl;
	std::cout << "Index file set to: " << indexFile << std::endl;
}


//20260210 Terto: define custom error page (error.html)
// main -> server.configureErrorPages() -> server.setErrorPage()
void Server::setErrorPage(int code, const std::string& filePath) 
{
	errorPages[code] = filePath;
}

//20260321 Alex: configures server_name for future storage to pollServer map<socket, Server>
//main->server.configureName();
void Server::configureServerName(const std::string &server_name)
{
	serverName = server_name;
}

//20260212 Terto: Configura IP, puerto, carpeta base e index
// main -> server.configureServer() -> server.listenOn() + server.setStaticRoot()
void Server::configureServer(const std::string& host, std::vector<int> ports, const std::string& root, const std::string& indexFile)
{
	try
	{
	setHost(host);
	setPorts(ports);
	// listenOn(ip, ports); Socket logic will be managed by pollServer
	setStaticRoot(root, indexFile);
	std::cout << " Root: " << root << " | Index: " << indexFile << std::endl;
	}

	catch (const std::runtime_error& e)
	{
		throw std::runtime_error("Failed to configure server");
	}
}


//20260225 Terto: Locations inside server configuration
// main -> server.configureLocations()
void Server::configureLocations(const std::vector<LocationConfig>& locations)
{
	this->locations = locations;
}


//20260227 Terto: Configure custom error pages dynamically
// main -> server.configureErrorPages() -> server.setErrorPage()
void Server::configureErrorPages(const std::string& root, const std::vector<std::string>& errorPaths)
{
	std::string cleanRoot = root;
	if (!cleanRoot.empty() && cleanRoot[cleanRoot.size() - 1] != '/')
		cleanRoot += '/';

	size_t i = 0;
	while (i < errorPaths.size())
	{
		std::string path = errorPaths[i];

		if (!path.empty() && path[path.size() - 1] == ';')
			path.erase(path.size() - 1);

		size_t slashPos = path.find_last_of('/');
		std::string filename = (slashPos != std::string::npos) ? path.substr(slashPos + 1) : path;
		int code = std::atoi(filename.substr(0, 3).c_str());

		if (!path.empty() && path[0] == '/')
			path.erase(0, 1);

		std::string fullPath = cleanRoot + path;
		setErrorPage(code, fullPath);

		++i;
	}

	std::cout << "Error pages configured." << std::endl;
}

// 20260319 Alex: simple debugging
void Server::printFinishedServerInfo()
{
	std::cout << std::endl << "--- Finished Server Info ---" << std::endl << std::endl;
	std::cout << "--- Server Name ---" << std::endl << getServerName() << std::endl;
	std::cout << "--- Host ---" << std::endl << getHost() << std::endl;
	std::cout << "--- Ports ---" << std::endl;
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++)
		std::cout << *it << std::endl;
	// std::cout << "--- Listen Sockets---" << std::endl;
	// for (std::vector<int>::iterator it = listenSockets.begin(); it != listenSockets.end(); it++)
	// 	std::cout << *it << std::endl;
	std::cout << "--- Static Root ---" << std::endl << staticRoot << std::endl;
	std::cout << "--- Index File ---" << std::endl << indexFile << std::endl;
	std::cout << "--- Error Pages ---" << std::endl;
	for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
		std::cout << it->first << it->second << std::endl;
	std::cout << "--- Location ---" << std::endl;
	for (size_t i = 0; i != locations.size() ; i++)
		locations[i].printLocation();
	std::cout << "--- End of Server info ---" << std::endl << std::endl;
}


