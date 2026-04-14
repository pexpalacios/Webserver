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

		// Store Server class into an array
		std::vector<Server> server_array;
		for (size_t i = 0; i < conf.size(); i++)
		{
			Server 			server;
			std::vector<int> ports = conf[i].getListen();
			server.configureServerName(conf[i].getServerName());
			server.configureServer(conf[i].getHost(), ports, conf[i].getRoot(), conf[i].getIndex());
			server.configureLocations(conf[i].getLocations());
			
			std::vector<LocationConfig> locations = server.getLocations();
			bool found = false;
			for (std::vector<LocationConfig>::iterator it = locations.begin(); it != locations.end(); ++it)
			{
				if (it->getPath() == "error")
				{
					server.configureErrorPages(conf[i].getRoot(), conf[i].getErrorPage());
					found = true;
					break;
				}
			}
			if (!found)
				server.configureErrorPages("", conf[i].getErrorPage());
			
			server.configureMaxBodySize(conf[i].getClientMaxSize());
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
