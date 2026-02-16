#include "../../includes/parse/ConfigParser.hpp"

ConfigParser::ConfigParser()
{}

ConfigParser::ConfigParser(const ConfigParser &copy)
{
	*this = copy;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &copy)
{
	if (this != &copy)
		*this = copy;
	return (*this);
}

///// AUXILIARY FUNCTIONS

std::string ConfigParser::omitSpaces(const std::string str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of("\t\r\n");

	if (start == std::string::npos)
		return ("");
	else
		return (str.substr(start, end - start + 1));
}

std::vector<std::string> ConfigParser::tokenize(const std::string &line)
{
	std::istringstream iss(line);
	std::vector<std::string> tokens;
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

static std::string stripSemicolon(const std::string &token)
{
	if (!token.empty() && token[token.size() - 1] == ';')
		return (token.substr(0, token.size() - 1));
	return (token);
}

///// SERVER VALUES SETTERS

static void setLocationBlockVars(LocationConfig &currentLocation, const std::string &key, const std::vector<std::string> &tokens)
{
	if (key == "root" && tokens.size() > 1)
		currentLocation.setRoot(stripSemicolon(tokens[1]));
	else if (key == "index" && tokens.size() > 1)
		currentLocation.setIndex(stripSemicolon(tokens[1]));
	else if (key == "upload" && tokens.size() > 1)
		currentLocation.setUpload(stripSemicolon(tokens[1]));
	else if (key == "autoindex" && tokens.size() > 1)
	{
		if (stripSemicolon(tokens[1]) == "on")
			currentLocation.setAutoindex(true);
		else if (stripSemicolon(tokens[1]) == "off")
			currentLocation.setAutoindex(false);
	}
	else if (key == "protected" && tokens.size() > 1)
	{
		if (stripSemicolon(tokens[1]) == "on")
			currentLocation.setProtected(true);
		else if (stripSemicolon(tokens[1]) == "off")
			currentLocation.setProtected(false);
	}
	else if (key == "allow_methods" && tokens.size() > 1)
	{
		std::vector<std::string> methods(tokens.begin() + 1, tokens.end());
		currentLocation.setMethods(methods);
	}
}

static void setServerBlockVars(ServerConfig &currentServer, const std::string &key, const std::vector<std::string> &tokens)
{
	if (key== "listen" && tokens.size() > 1)
		currentServer.setListen(std::atoi(tokens[1].c_str()));
	else if (key == "host" && tokens.size() > 1)
		currentServer.setHost(stripSemicolon(tokens[1]));
	else if (key == "server_name" && tokens.size() > 1)
		currentServer.setServerName(stripSemicolon(tokens[1]));
	else if (key == "error_page" && tokens.size() > 2)
		currentServer.addErrorPage(stripSemicolon(tokens[2]));
	else if (key == "root" && tokens.size() > 1)
		currentServer.setRoot(stripSemicolon(tokens[1]));
	else if (key == "index" && tokens.size() > 1)
		currentServer.setIndex(stripSemicolon(tokens[1]));
	else if (key == "client_max_body_size" && tokens.size() > 1)
	{
		std::string len = tokens[1];
		size_t size = 0;
		if (len.find("mb") != std::string::npos)
			size = std::atoi(len.c_str()) * 1024 * 1024;
		else
			size = std::atoi(len.c_str());
		currentServer.setClientMaxSize(size);
	}
}

static void cleanPageUrl(ServerConfig &currentServer)
{
	int len = currentServer.getRoot().length();
	currentServer.setIndex(currentServer.getIndex().substr(len, currentServer.getIndex().length()));

	std::vector<std::string> errorPages = currentServer.getErrorPage();
	std::vector<std::string> newPages;
	for (std::vector<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		newPages.push_back(it->substr(len));
	currentServer.setErrorPage(newPages);
}

///// MAIN PARSING FUNCTION

std::vector<ServerConfig> ConfigParser::parse(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw (std::runtime_error("Couldn't open config file"));

	std::string line;
	bool inServer = false;
	bool inLocation = false;
	ServerConfig currentServer;
	LocationConfig currentLocation;
	std::vector<ServerConfig> servers;

	try
	{
		while (std::getline(file, line))
		{
			line = omitSpaces(line);
			if (line.empty())
				continue;

			if (line.find("server") == 0 && line.find("{") != std::string::npos)
			{
				if (inServer)
					throw (std::runtime_error("Nested server blocks"));
				inServer = true;
				currentServer = ServerConfig();
				continue;
			}

			if (inServer)
			{
				if (line.find("location") == 0 && line.find("{") != std::string::npos)
				{
					if (inLocation)
						throw (std::runtime_error("Nested location blocks"));
					inLocation = true;
					std::vector<std::string> tokens = tokenize(line);
					if (tokens.size() < 3)
						throw (std::runtime_error("Invalid location block"));
					currentLocation = LocationConfig();
					currentLocation.setPath(tokens[1]);
					if (line.find("}") != std::string::npos)
					{
						currentServer.addLocation(currentLocation);
						inLocation = false;
						continue;
					}
					continue;
				}
				if (inLocation && line == "}")
				{
					currentServer.addLocation(currentLocation);
					inLocation = false;
					continue;
				}

				std::vector<std::string> tokens = tokenize(line);
				if (tokens.empty())
					continue;
				std::string key = tokens[0];
				if (inLocation)
					setLocationBlockVars(currentLocation, key, tokens);
				else
					setServerBlockVars(currentServer, key, tokens);
			}
			if (inServer && line == "}")
			{
				if (inLocation)
					throw (std::runtime_error("Location not closed"));
				cleanPageUrl(currentServer);
				checkServerValues(currentServer);
				servers.push_back(currentServer);
				inServer = false;
				continue;
			}
		}
		if (inServer)
			throw (std::runtime_error("Unclosed server block"));
		if (!inServer && servers.empty())
			throw (std::runtime_error("Config file is empty"));
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return (servers);
}
