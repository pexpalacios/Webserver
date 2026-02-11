#include "../../includes/parse/ConfigParser.hpp"

ConfigParser::ConfigParser()
{

}

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

////////////

static std::string stripSemicolon(const std::string &token)
{
	if (!token.empty() && token.back() == ';') //it says there's no element .back in a std::string :/
		return (token.substr(0, token.size() - 1));
	return (token);
}

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
		if (tokens[1] == "on")
			currentLocation.setAutoindex(true);
		else if (tokens[1] == "off")
			currentLocation.setAutoindex(false);
	}
	else if (key == "protected" && tokens.size() > 1)
	{
		if (tokens[1] == "on")
			currentLocation.setProtected(true);
		else if (tokens[1] == "off")
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
		currentServer.setErrorPage(stripSemicolon(tokens[2]));
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
			//std::cout << " : " << line << std::endl;
			if (line.empty()) //add if a line is a comment? check for # or //
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
				//check if inside a location
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
				// tokenize the line and vars to corresponding block
				std::vector<std::string> tokens = tokenize(line);
				if (tokens.empty())
					continue;
				std::string key = tokens[0];
				if (inLocation)
					setLocationBlockVars(currentLocation, key, tokens);
				else
					setServerBlockVars(currentServer, key, tokens);
			}
			//check the last line is }
			if (inServer && line == "}")
			{
				if (inLocation)
					throw (std::runtime_error("Location not closed"));
				servers.push_back(currentServer);
				inServer = false;
				continue;
			}
		}
		currentServer.printServer();
		checkServerValues(currentServer);
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

///////////

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

////////////////

static int isValidIPv4(const std::string &ip)
{
	int dots = 0;
	int nums = 0;

	for (size_t i = 0; i < ip.size(); i++)
	{
		if (!std::isdigit(ip[i]) && ip[i] != '.')
			return (0);
		int n = 0;
		while (std::isdigit(ip[i]))
		{
			n = n * 10 + (ip[i] - '0');
			i++;
		}
		if (ip[i] == '.')
			dots++;
		if (!(n >= 0 && n <= 255))
			return (0);
		else
			nums++;
	}

	if (dots != 3 || nums != 4)
		return (0);
	return (1);
}

static int findServer(std::string name)
{
	struct stat st;
	std::string path = "/www/" + name;
	if (lstat(path.c_str(), &st) == 0)
	{
		if (!S_ISDIR(st.st_mode))
			return (0);
	}
	else
		return (0);
	return (1);
}

static int findPage(std::string name, std::string server)
{
	if (name.length() < 5 || name.substr(name.length() - 5) != ".html")
        return (0);

	struct stat st;
	std::string path = "/www/" + server + "/" + name;
	if (lstat(path.c_str(), &st) == 0)
	{
		if (!S_ISREG(st.st_mode))
			return (0);
	}
	else
		return (0);
	return (1);
}



void ConfigParser::checkServerValues(ServerConfig &server)
{
	std::cout << std::endl;
	std::cout << "[ERROR CHECKS]" << std::endl;
	if (std::isdigit(server.getListen()))
		std::cout << "Listen in server: " << server.getServerName() << " is not numeric" << std::endl;
	if (!(server.getListen() >= 1 && server.getListen() <= 65535))
		std::cout << "Listen is server: " << server.getServerName() << " is out of listening range" << std::endl;

	//host needs to check that it's 4 numbers separated by "." and minimun and maximun
	if (!isValidIPv4(server.getHost()))
		std::cout << "Host in server: " << server.getServerName() << " is not a valid IPv4 value" << std::endl;
	//server_name needs to be compatible with a directory in www/
	if (!findServer(server.getServerName()))
		std::cout << "Server: " << server.getServerName() << " does not exist or is innaccesible" << std::endl;
	//error_page must be 404 and be an .html in www/servername/error/
	if (!findPage(server.getErrorPage(), server.getServerName()))
		std::cout << "Page: " << server.getErrorPage() << " does not exist in Server: " << server.getServerName() << std::endl;
	//clientmaxbodysie must be a number with minimun and maximun
	//root must be a directory www/serveername
	//index must be a .html in root directory
}