#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>			// std::ifstream
#include <poll.h>			// poll() y struct pollfd
#include <csignal>			// signal handling
#include <algorithm>		// std::find
#include <sstream>			// std::stringstream
#include <map>				// std::map

#include "parse/ConfigParser.hpp"
#include "parse/LocationConfig.hpp"
#include "parse/ServerConfig.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"

#include "Signal.hpp"


//20260210 Terto: Server class definition
class Server
{
public:
	Server();
	~Server();

	void	configureServer(const std::string& ip, int port, const std::string& root, const std::string& indexFile);
	void	configureErrorPages(const std::string& root, std::string error_404, std::string error_500);
	void	handleNewConnection(int listenSock, std::vector<struct pollfd>& fds);
	void	handleClientConnection(int clientSock, Server& server);
	void	run();

private:
	void						listenOn(const std::string& ip, int port);								
	void						setStaticRoot(const std::string& root, const std::string& indexFile);
	void						setErrorPage(int code, const std::string& path);						
	std::vector<struct pollfd>	buildPollFdArray();														
	std::string					readFile(const std::string& path);										

	std::vector<int>			listenSockets;	// Sockets en los que el servidor está escuchando
	std::string					staticRoot;		// Carpeta base para archivos estáticos
	std::string 				indexFile;		// Archivo index (index.html)
	std::map<int, std::string>	errorPages;		// Mapa de códigos de error y rutas a sus páginas

};

#endif
