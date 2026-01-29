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

//20260129
// Gestiona el servidor y puerto que escucha + Carpeta con archivos estáticos + bucle inicial
class Server
{
public:
	Server();
	~Server();

private:
	std::vector<int>	listenSockets;							// Lista de sockets en escucha

	void	listenOn(const std::string& ip, int port);			// Configura socket del servidor para escuchar IP y puerto concretos
	void	setStaticRoot(const std::string& root);				// Define la carpeta base (www/index.html)
	void	run();												// Inicia el bucle principal del servidor
};


#endif