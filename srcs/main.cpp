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

	return (0);
}