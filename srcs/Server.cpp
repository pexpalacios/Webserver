#include "../includes/Server.hpp"
#include "../includes/handleConn.hpp"

// 20260129 Terto: Variable global (TEMPORALMENTE) para controlar el bucle del servidor
volatile	sig_atomic_t server_running = 1;


Server::Server(){}
Server::~Server()
{}

// 20260129 Terto: Configura socket para escuchar IP y puerto
// main -> server::listenOn(ip, port);
void Server::listenOn(const std::string& ip, int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) 
	{
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


// 20260129 Terto: Establish the static root folder for serving files
// main -> server::setStaticRoot("www");
void Server::setStaticRoot(const std::string& root) {
	staticRoot = root;
	std::cout << "Static root set to: " << staticRoot << std::endl;
}


// 20260207 Terto: Read the content of a file and return it as a string
// main -> server::run() -> readFile(filePath);
std::string Server::readFile(const std::string& path) {
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error (0.1) Failed to open file: " << path << std::endl;
		return "";
	}

	std::string content;
	std::string line;
	while (std::getline(file, line))
		content += line + "\n";

	file.close();
	return content;
}


// 20260207 Terto: Start the main server loop to accept and handle client connections
// main -> Server::run()
void Server::run()
{
	// 20260207 Terto: Handle signals: SIGINT (Ctrl+C)
	signal(SIGINT, handleSignal);

	// 20260207 Terto: Set up pollfd array for all sockets in listenSockets
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

	// 20260207 Terto: Main server loop
	while (server_running)
	{
		// 20260207 Terto: Wait for events on all sockets
		int ret = poll(&fds[0], fds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "Error (0.1): poll() failed." << std::endl;
			break;
		}

		// 20260207 Terto: Check which sockets have events
		size_t j = 0;
		while (j < fds.size())
		{
			if (fds[j].revents & POLLIN)
			{
				// 20260207 Terto: Check if it's a new connection on a listen socket
				if (std::find(listenSockets.begin(), listenSockets.end(), fds[j].fd) != listenSockets.end())
				{
					handleNewConnection(fds[j].fd, fds);
				}
				// 20260207 Terto: Otherwise, it's an existing client connection
				else
				{
					handleClientConnection(fds[j].fd, staticRoot);
					fds.erase(fds.begin() + j);
					continue;
				}
			}
			++j;
		}
	}
}
