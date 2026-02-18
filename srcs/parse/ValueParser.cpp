#include "../../includes/parse/ConfigParser.hpp"

static int isValidIPv4(const std::string &ip)
{
	int dots = 0;
	int nums = 0;

	for (size_t i = 0; i < ip.size(); i++)
	{
		if (!std::isdigit(ip[i]) && ip[i] != '.')
			return (0);
		int n = 0;
		while (i < ip.size() && std::isdigit(ip[i]))
		{
			n = n * 10 + (ip[i] - '0');
			i++;
		}
		if (i < ip.size() && ip[i] == '.')
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
	std::string path = "./www/" + name;
	if (lstat(path.c_str(), &st) == 0)
	{
		if (!S_ISDIR(st.st_mode))
			return (0);
	}
	else
		return (0);
	return (1);
}

static int isValidExtension(std::string name, std::string extension)
{
	size_t len = extension.length();
	if ((name.length() < len) || name.substr(name.length() - len) != extension.c_str())
		return (0);
	return (1);
}

static int findPage(std::string name, std::string root)
{	
	if (!isValidExtension(name, ".html"))
		return (0);

	std::string fullurl = root + name;
	struct stat st;
	if (lstat(fullurl.c_str(), &st) == 0)
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
	std::vector<int> ports = server.getListen();
	for (std::vector<int>::iterator it = ports.begin(); it < ports.end(); ++it)
	{
		if (!(*it >= 1 && *it <= 65535))
			throw std::invalid_argument("Listen in server: " + server.getServerName() + " is out of listening range");
	}
	if (!isValidIPv4(server.getHost()))
		throw std::invalid_argument("Host in server: " + server.getServerName() + " is not a valid IPv4 value");

	if (!findServer(server.getServerName()))
		throw std::invalid_argument("Server: " + server.getServerName() + " does not exist or is innaccesible");

	std::vector<std::string> errorPages = server.getErrorPage();
	for (size_t i = 0; i < errorPages.size(); i++)
	{
		if (!errorPages[i].empty() && !findPage(errorPages[i], server.getRoot()))
			throw std::invalid_argument("Page: " + errorPages[i] + " does not exist in Server: " + server.getServerName());
	}

	if (server.getClientmaxSize() == 0)
		throw std::invalid_argument("Client max body size in server: " + server.getServerName() + " is zero");
	if (server.getClientmaxSize() > 1073741824) // 1GB max
		throw std::invalid_argument("Client max body size in server: " + server.getServerName() + " exceeds maximum (1GB)");

	struct stat st;
	if (lstat(server.getRoot().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw std::invalid_argument("Root directory: " + server.getRoot() + " does not exist or is not a directory");
	if (!findPage(server.getIndex(), server.getRoot()))
		throw std::invalid_argument("Index page: " + server.getIndex() + " does not exist");

	std::vector<LocationConfig> locations = server.getLocations();
	for (std::vector<LocationConfig>::iterator it = locations.begin(); it != locations.end(); ++it)
		checkLocationValues(*it);
}

///////////////

void ConfigParser::checkLocationValues(LocationConfig &location)
{
	struct stat st;
	if (lstat(location.getPath().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw std::invalid_argument("Location path: " + location.getPath() + " doesn't exist or is innaccesible");
	if (lstat(location.getRoot().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw std::invalid_argument("Root on location: " + location.getPath() + " doesn't exist or is innaccesible");
	if (lstat(location.getUpload().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw std::invalid_argument("Upload on location: " + location.getPath() + " doesn't exist or is innaccesible");
	if (!findPage(location.getIndex(), location.getPath()))
		throw std::invalid_argument("Index page on location: " + location.getPath() + " doesn't exist or is innaccesible");

	if (location.getAutoindex() != 0 && location.getAutoindex() != 1)
		throw std::invalid_argument("Autroindex on location: " + location.getPath() + " is invalid (must be 'on' or 'off')");
	if (location.getProtected() != 0 && location.getProtected() != 1)
		throw std::invalid_argument("Protected on location: " + location.getPath() + " is invalid (must be 'on' or 'off')");
	
	//It must be GET POST and DELETE
	std::vector<std::string> methods = location.getMethods();
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			throw std::invalid_argument("Methods on location: " + location.getPath() + " is invalid (must be 'GET', 'POST', 'DELETE')");

	//I think this one should only check the cgi-bin folder
	std::vector<std::string> cgipath = location.getCGIPath();
	for (std::vector<std::string>::iterator it = cgipath.begin(); it != cgipath.end(); ++it)
		if (lstat(it->c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			throw std::invalid_argument("CGIPath on location: " + location.getPath() + " doesn't exist or is innaccesible");

	//check .py and .sh and .php
	std::vector<std::string> cgiexts = location.getCGIExt();
	for (std::vector<std::string>::iterator it = cgiexts.begin(); it != cgiexts.end(); ++it)
		if (*it != ".py" && *it != ".sh" && *it != ".php")
			throw std::invalid_argument("CGIExtension on location: " + location.getPath() + " is invalid (only accepts '.py', '.sh', '.php')");
}