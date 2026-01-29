#include "../includes/Server.hpp"	//20260129 Terto: Clases para el servidor

Server::Server(){}
Server::~Server()
{}

void Server::listenOn(const std::string& ip, int port)
{
	// 20260129 Terto: Configura socket para escuchar IP y puerto concretos
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) 
	{
		std::cerr << "Error (0.1): creating socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	// 20260129 Terto: Configura socket para no bloquearse en espera
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed (0.2): set non-blocking");

	// 20260129 Terto: Configura socket para reutilizar la dirección
	int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("Failed (0.3): setsockopt");

	// 20260129 Terto: Asocia la dirección IP y el puerto al socket
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	// 20260129 Terto: Asocia la dirección IP y el puerto al socket
	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Failed (0.4): bind");

	// 20260129 Terto: Escucha conexiones entrantes
	if (listen(sock, 100) == -1)
		throw std::runtime_error("Failed (0.5): listen");

	// 20260129 Terto: Agrega el socket a la lista de sockets en escucha
	listenSockets.push_back(sock);
	std::cout << "Listening on " << ip << ":" << port << std::endl;
}

void Server::setStaticRoot(const std::string& root)
{
	// Define la carpeta base (www/index.html)
}


void Server::run()
{
	// Inicia el bucle principal del servidor
}
