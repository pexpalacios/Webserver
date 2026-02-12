//20260210 Terto includes
#include "../includes/Server.hpp"
#include "../includes/Signal.hpp"

//20260212 Terto: main
int main() 
{
	SignalHandler::registerSignal();

	Server 			server;
	std::string		ip						= "127.0.0.1";
	int				port					= 8080;
	std::string		server_name				= "webserv";
	std::string		root					= "www";
	std::string		index					= "index.html";
	std::string		error_404				= "error_pages/404.html"; 
	std::string		error_500				= "error_pages/500.html";
	size_t			client_max_body_size	= 1024 * 1024; // 1 MB


	(void)client_max_body_size; // Variable no utilizada en esta versión
	server.configureServer(ip, port, root, index);
	server.configureErrorPages(error_404, error_500);
	server.run();
	return 0;
}
