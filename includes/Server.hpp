#ifndef SERVER_HPP
	#define SERVER_HPP

#include "library.hpp"
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

	void		configureServer(const std::string& ip, std::vector<int> ports, const std::string& root, const std::string& indexFile);
	void		configureLocations(const std::vector<LocationConfig>& locations);
	void		configureErrorPages(const std::string& root, const std::vector<std::string>& errorPaths);
	void		handleNewConnection(int listenSock, std::vector<struct pollfd>& fds);
	void		handleClientConnection(int clientSock, Server& server);
	std::string	recvRequest(int clientSock);
	void		run();	
  void    closeSockets();


	const std::string&	getStaticRoot() const;
	const std::string&	getIndexFile() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::vector<LocationConfig>& getLocations() const;

private:
	void						listenOn(const std::string& ip, std::vector<int> ports);								
	void						setStaticRoot(const std::string& root, const std::string& indexFile);
	void						setErrorPage(int code, const std::string& path);						
	std::vector<struct pollfd>	buildPollFdArray();														
	std::string					readFile(const std::string& path);										

	std::vector<int>			listenSockets;	// Sockets en los que el servidor está escuchando
	std::string					staticRoot;		// Carpeta base para archivos estáticos
	std::string 				indexFile;		// Archivo index (index.html)
	std::map<int, std::string>	errorPages;		// Mapa de códigos de error y rutas a sus páginas
	std::vector<LocationConfig>	locations;		// Configuraciones de ubicación (locations)
};

# endif
