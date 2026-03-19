#include "../includes/Server.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "Invalid number of arguments" << std::endl;
		return (1);
	}
	SignalHandler::registerSignal();

	try
	{
		ConfigParser parser;
		std::vector<ServerConfig> conf;
		if (ac == 2)
			conf = parser.parse(av[1]);
		else 
			conf = parser.parse("./config/default.conf");

		std::cout << "===Server conf file===" << std::endl << std::endl;
		for (size_t i = 0; i < conf.size(); i++){
			std::cout << "===Server " << i << "===" << std::endl;
			conf[i].printServer();
		}
		std::cout << "===End conf file===" << std::endl << std::endl;

		// We do this on a loop for a vector of Servers	
		// std::vector<Server> server_array;
		//{
		Server 			server;
		server.printFinishedServerInfo();
		std::vector<int> ports = conf[0].getListen();
		server.configureServer(conf[0].getHost(), ports, conf[0].getRoot(), conf[0].getIndex());
		server.configureErrorPages(conf[0].getRoot(), conf[0].getErrorPage());
		server.configureLocations(conf[0].getLocations());
		server.printFinishedServerInfo();
		// server_array.pushback(server)
		// }
		// GlobalServer global_server(server_array);
		server.run();
	} 
	catch (const std::invalid_argument& e)
	{
		std::cerr << "Error in config parsing:\n" << e.what() << std::endl;
		return (1);
	}
	return 0;
}
