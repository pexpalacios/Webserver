#include "../../includes/parse/ConfigParser.hpp"

ConfigParser::ConfigParser()
{

}

ConfigParser::ConfigParser(const ConfigParser &copy)
{

}

ConfigParser &ConfigParser::operator=(const ConfigParser &copy)
{

}

////////////

std::vector<ServerConfig> ConfigParser::parse(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw (std::runtime_error("Couldn't open config file"));

	//read filename
	//check if filename has keywords (listen, root, index, locations, host, etc)
	//create a vector or some struct that contains all keywords
	//create a ServerConfig obj when keyword "server" is found
	//compare to new struct to check if the value is valid (maybe a map?)
	//if value is valid assign it to a ServerConfig obj
	//if not, throw exception (create my own exception in class? or is invalid_argument enough?)
	//add obj to a vector and return it

	std::vector<ServerConfig> servers;
	std::string line;
	bool inServer = false;
	bool inLocation = false;
	ServerConfig currentServer;
	LocationConfig currentLocation;

	while (std::getline(file, line))
	{
		line = omitSpaces(line); //create omitSpaces function to clean line
		if (line.empty()) //add if a line is a comment? check for # or //
			continue;

		if (line.find("server") == 0 && line.find("{") != std::string::npos)
		{
			if (inServer)
				throw (std::runtime_error("Server inside server"));
			inServer = true;
			currentServer = ServerConfig();
			continue;
		}

		if (inServer && line == "}")
		{
			if (inLocation)
				throw (std::runtime_error("Location not closed"));
			servers.push_back(currentServer);
			inServer = false;
			continue;
		}

		if (inServer)
		{
			//check if inside a location
			//create a token vector and tokenize the line
		}

		//check the closing of locations

	}
	return (servers);
}