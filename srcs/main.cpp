//20260210 Terto includes
#include "../includes/Server.hpp"


//20260212 Terto: main
int main(int ac, char **av)
{
  	if (ac != 2)
	{
		std::cout << "Invalid number of arguments" << std::endl;
		return (1);
	}
	SignalHandler::registerSignal();

	try
	{
		ConfigParser parser;
		std::vector<ServerConfig> conf = parser.parse(av[1]);

		Server 			server;
		std::vector<int> ports = conf[0].getListen();
		server.configureServer(conf[0].getHost(), ports[0], conf[0].getRoot(), conf[0].getIndex());
		server.configureErrorPages(conf[0].getRoot(), conf[0].getErrorPage()[0], conf[0].getErrorPage()[1]);
		server.run();
	} 
	catch (const std::invalid_argument& e)
	{
		std::cerr << "Error in config parsing:\n" << e.what() << std::endl;
		return (1);
	}
	return 0;
}
