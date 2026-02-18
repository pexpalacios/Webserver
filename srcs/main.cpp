#include "../includes/Server.hpp"


int main(int ac, char **av)
{
  	if (ac != 2)
	{
		std::cout << "Invalid number of arguments" << std::endl;
		return (1);
	}
	ConfigParser parser;
	std::vector<ServerConfig> conf = parser.parse(av[1]);
	SignalHandler::registerSignal();

	Server 			server;

	//(void)client_max_body_size; // Variable no utilizada en esta versión
	server.configureServer(conf[0].getHost(), conf[0].getListen(), conf[0].getRoot(), conf[0].getIndex());
	server.configureErrorPages(conf[0].getRoot(), conf[0].getErrorPage()[0], conf[0].getErrorPage()[1]);
	server.run();
	return 0;
}
