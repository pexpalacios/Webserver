#include "../includes/Server.hpp"
#include "../includes/handleConn.hpp"
#include "../includes/Signal.hpp"

Server::Server() {}
Server::~Server() {}


//20260210 Terto: configure server socket to listen on specific IP and port
// main -> server.listenOn(ip, port)
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


//20260210 Terto: define static root folder for serving static files (www/index.html)
// main -> server.setStaticRoot("www")
void Server::setStaticRoot(const std::string& root) 
{
	staticRoot = root;
	std::cout << "Static root set to: " << staticRoot << std::endl;
}

//20260210 Terto: define custom error page (error.html)
// main -> server.setErrorPage(code, path)
void Server::setErrorPage(int code, const std::string& filePath) 
{
	errorPages[code] = filePath;
	std::cout << "Error page " << code << " set to: " << filePath << std::endl;
}


//20260210 Terto: read file content and return as string
// main -> server.run()
std::string Server::readFile(const std::string& path) 
{
	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		std::cerr << "Error (0.1): Failed to open file: " << path << std::endl;
		return "";
	}

	std::string content;
	std::string line;
	while (std::getline(file, line))
		content += line + "\n";

	file.close();
	return content;
}


// 20260210 Terto: Build and return pollfd array from listenSockets
// main -> run() -> buildPollFdArray()
std::vector<struct pollfd> Server::buildPollFdArray()
{
	std::vector<struct pollfd> fds;
	size_t i = 0;
	while (i < listenSockets.size()) 
	{
		struct pollfd pfd;
		pfd.fd = listenSockets[i];
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
		++i;
	}
	std::cout << "Waiting for connections..." << std::endl;
	return fds;
}



//20260210 Terto: Inicia el bucle principal del servidor, usando poll() para manejar múltiples conexiones
// main -> server.run()
void Server::run() 
{
	// Agrega los sockets de escucha al array de pollfd
	std::vector<struct pollfd> fds = buildPollFdArray();


	// main loop with SIGINT (Ctrl+C) to stop the server
	while (SignalHandler::running == 1)
	{
		int ret = poll(&fds[0], fds.size(), -1);
		if (ret < 0) {
			std::cerr << "Error (0.1): poll() failed." << std::endl;
			break;
		}

		// Check for events on each socket
		size_t j = 0;
		while (j < fds.size()) {
			if (fds[j].revents & POLLIN) 
			{
				if (std::find(listenSockets.begin(), listenSockets.end(), fds[j].fd) != listenSockets.end()) 
				{
					handleNewConnection(fds[j].fd, fds);
				} 
				else 
				{
					handleClientConnection(fds[j].fd, *this);
					fds.erase(fds.begin() + j);
					continue;
				}
			}
			++j;
		}
	}
}
