#include "../includes/Server.hpp"
#include "../includes/PollServer.hpp"

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
		// Parse conf. file
		ConfigParser parser;
		std::vector<ServerConfig> conf;
		if (ac == 2)
			conf = parser.parse(av[1]);
		else 
			conf = parser.parse("./config/default.conf");


		// Check server .conf Files
		// std::cout << "===Server conf file===" << std::endl << std::endl;
		// for (size_t i = 0; i < conf.size(); i++){
		// 	std::cout << std::endl << "===Server " << i << "===" << std::endl;
		// 	conf[i].printServer();
		// }
		// std::cout << "===End conf file===" << std::endl << std::endl;


		// Store Server class into an array
		std::vector<Server> server_array;
		for (size_t i = 0; i < conf.size(); i++)
		{
			Server 			server;
			std::vector<int> ports = conf[i].getListen();
			server.configureServerName(conf[i].getServerName());
			server.configureServer(conf[i].getHost(), ports, conf[i].getRoot(), conf[i].getIndex());
			server.configureErrorPages(conf[i].getRoot(), conf[i].getErrorPage());
			server.configureLocations(conf[i].getLocations());
			server.configureMaxBodySize(conf[i].getClientMaxSize());
			//server.printFinishedServerInfo();
			server_array.push_back(server);
		}

		// Add server_array into PollServer and init poll() logic
		PollServer pollServer(server_array);
		pollServer.buildPollServerArray();
		pollServer.run();
		
	} 
	catch (const std::runtime_error& e)
	{
		std::cerr << "Error in configure server:\n" << e.what() << std::endl;
		return (1);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "Error in config parsing:\n" << e.what() << std::endl;
		return (1);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Another type of error occurred:\n" << e.what() << std::endl;
		return (1);
	}
	return 0;
}
