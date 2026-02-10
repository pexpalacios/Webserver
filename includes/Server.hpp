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

// 20260207 Terto: Clase para gestionar el servidor, puerto que escucha, carpeta con archivos estáticos y bucle inicial
class Server
{
public:
	Server();
	~Server();

	void			listenOn(const std::string& ip, int port);     // Configura socket del servidor para escuchar IP y puerto concretos
	void			setStaticRoot(const std::string& root);        // Define la carpeta base (www/index.html)
	void			setErrorPage(int code, const std::string& path); // Define la ruta personalizada para errores
	void			run();                                         // Inicia el bucle principal del servidor

	std::string					readFile(const std::string& path);            // Lee el contenido de un archivo y lo devuelve como string
	std::vector<int>			listenSockets;                                // Lista de sockets en escucha
	std::string					staticRoot;                                   // Carpeta base para archivos estáticos
	std::map<int, std::string>	errorPages;                            // Mapa de códigos de error y rutas a sus páginas

private:
	static			Server* instance;                             // puntero estático a la instancia
	volatile		sig_atomic_t server_running;                  // Variable de control del bucle
	void			handleSignal(int signal);                     // Manejador de señales (SIGINT)
};

#endif
