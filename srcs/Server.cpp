#include "../includes/Server.hpp"
#include "../includes/Signal.hpp"

Server::Server() {}
Server::~Server() {}

const std::string& Server::getStaticRoot() const
{return staticRoot;}

const std::string& Server::getIndexFile() const
{return indexFile;}

const std::map<int, std::string>& Server::getErrorPages() const
{return errorPages;}

//20260212 Terto: configure server socket to listen on specific IP and port
// main -> server.configureServer() -> server.listenOn()
void Server::listenOn(const std::string& ip, int port) 
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		std::cerr << "Error (0.1): creating socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed (0.2): set non-blocking");

	int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Failed (0.3): setsockopt");

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Failed (0.4): bind");
	if (listen(sock, 100) == -1)
		throw std::runtime_error("Failed (0.5): listen");

	listenSockets.push_back(sock);
	std::cout << "Listening on " << ip << ":" << port << std::endl;
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
	std::cout << "Error page " << code << " set to: " << filePath << std::endl;
}


//20260212 Terto: Configura IP, puerto, carpeta base e index
// main -> server.configureServer() -> server.listenOn() + server.setStaticRoot()
void Server::configureServer(const std::string& ip, int port, const std::string& root, const std::string& indexFile)
{
	listenOn(ip, port);
	setStaticRoot(root, indexFile);
	std::cout << " Root: " << root << " | Index: " << indexFile << std::endl;
}


//20260223 Terto: Asocia páginas de error personalizadas
// main -> server.configureErrorPages() -> server.setErrorPage()
void Server::configureErrorPages(const std::string& root, std::string error_404, std::string error_500)
{
	if (!error_404.empty() && error_404[error_404.size() - 1] == ';')
		error_404.erase(error_404.size() - 1);

	if (!error_500.empty() && error_500[error_500.size() - 1] == ';')
		error_500.erase(error_500.size() - 1);

	if (!error_404.empty() && error_404[0] == '/')
		error_404.erase(0, 1);

	if (!error_500.empty() && error_500[0] == '/')
		error_500.erase(0, 1);

	std::string cleanRoot = root;

	if (!cleanRoot.empty() && cleanRoot[cleanRoot.size() - 1] != '/')
		cleanRoot += '/';

	std::string full404 = cleanRoot + error_404;
	std::string full500 = cleanRoot + error_500;
	setErrorPage(404, full404);
	setErrorPage(500, full500);

	std::cout << "Error pages configured." << std::endl;
}
