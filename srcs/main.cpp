//20260210 Terto includes
#include "../includes/Server.hpp"
#include "../includes/handleConn.hpp"
#include "../includes/Signal.hpp"

//20260210 Terto: main
int main() 
{
	SignalHandler::registerSignal();

	Server 			server;
	std::string		ip		= "127.0.0.1";
	int				port	= 8080;

	server.listenOn(ip, port);								// Configura el socket del servidor para que escuche en una IP y puerto concretos
	server.setStaticRoot("www");							// Define la carpeta base (www/index.html)
	server.setErrorPage(404, "error_pages/404.html");		// Define la página de error 404 (error.html)
	server.setErrorPage(500, "error_pages/500.html");		// Define la página de error 500 (error.html)
	server.run();											// Inicia el bucle principal del servidor

	return (0);
}
