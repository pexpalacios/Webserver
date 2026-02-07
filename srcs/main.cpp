#include "../includes/Server.hpp"
#include "../includes/handleConn.hpp"
int main() 
{
	Server 			server;
	std::string		ip		= "127.0.0.1";
	int				port	= 8080;

	server.listenOn(ip, port);		// Configura el socket del servidor para que escuche en una IP y puerto concretos
	server.setStaticRoot("www");	// Define la carpeta base (www/index.html)

	// 20260207 Terto: 
	server.run();					// Inicia el bucle principal del servidor

	return 0;
}
