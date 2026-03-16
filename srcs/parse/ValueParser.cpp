#include "../../includes/parse/ConfigParser.hpp"

//This whole file is part of the ConfigParser class. It only has comprobations for valid values

//This checks if a IP/host is valid (contains 4 numbers and 3 dots)
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

//This checks if the server exists in the directory /www
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

//This checks if an extension is valid, was in findPage before but i though it could be used for other extensions
static int isValidExtension(std::string name, std::string extension)
{
	size_t len = extension.length();
	if ((name.length() < len) || name.substr(name.length() - len) != extension)
		return (0);
	return (1);
}

//This checks is a file exists wihin the server
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

//This gets rid of the whole URL of a page, leaving just the file's name
static std::string cleanPageUrl(std::string url, std::string root)
{
	size_t len = root.length();
	if (len > url.length())
		return ("");
	std::string newstr = url.substr(len, url.length());
	return (newstr);
}

//// MAIN ONES
//This checks all possible server values to see if they are valid
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
		errorPages[i] = cleanPageUrl(errorPages[i], server.getRoot());
		if (!errorPages[i].empty() && !findPage(errorPages[i], server.getRoot()))
			throw std::invalid_argument("Page: " + errorPages[i] + " does not exist in Server: " + server.getServerName());
	}
	server.setErrorPage(errorPages);

	std::cout << "Mb: " << server.getClientmaxSize() << std::endl;
	if (server.getClientmaxSize() == 0)
		throw std::invalid_argument("Client max body size in server: " + server.getServerName() + " is zero");
	if (server.getClientmaxSize() > 1073741824) // 1GB max
		throw std::invalid_argument("Client max body size in server: " + server.getServerName() + " exceeds maximum (1GB)");

	struct stat st;
	if (lstat(server.getRoot().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw std::invalid_argument("Root directory: " + server.getRoot() + " does not exist or is not a directory");
	server.setIndex(cleanPageUrl(server.getIndex(), server.getRoot()));

	if (!findPage(server.getIndex(), server.getRoot()))
		throw std::invalid_argument("Index page: " + server.getIndex() + " does not exist");

	std::vector<LocationConfig> locations = server.getLocations();
	for (std::vector<LocationConfig>::iterator it = locations.begin(); it != locations.end(); ++it)
		checkLocationValues(*it, server.getRoot());
}

///////////////
//This checks all possible location values to see if they are valid
void ConfigParser::checkLocationValues(LocationConfig &location, const std::string root)
{
	struct stat st;

	if (!location.getPath().empty())
	{
		std::string fullPath = root + location.getPath();
		if (lstat(fullPath.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			throw std::invalid_argument("Location path: " + location.getPath() + " doesn't exist or is innaccesible");
	}
	if (!location.getRoot().empty())
		if (lstat(location.getRoot().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			throw std::invalid_argument("Root on location: " + location.getPath() + " doesn't exist or is innaccesible");
	if (!location.getUpload().empty())
		if (lstat(location.getUpload().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
			throw std::invalid_argument("Upload on location: " + location.getPath() + " doesn't exist or is innaccesible");

	location.setIndex(cleanPageUrl(location.getIndex(), root));
	if (!location.getIndex().empty())
		if (!findPage(location.getIndex(), root))
			throw std::invalid_argument("Index page: " + location.getIndex() + " on location: " + location.getPath() + " doesn't exist or is innaccesible");

	if (location.getAutoindex() != 0 && location.getAutoindex() != 1)
		throw std::invalid_argument("Autoindex on location: " + location.getPath() + " is invalid (must be 'on' or 'off')");
	if (location.getProtected() != 0 && location.getProtected() != 1)
		throw std::invalid_argument("Protected on location: " + location.getPath() + " is invalid (must be 'on' or 'off')");

	std::vector<std::string> methods = location.getMethods();
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			throw std::invalid_argument("Methods on location: " + location.getPath() + " is invalid (must be 'GET', 'POST', 'DELETE')");

	// std::vector<std::string> cgipath = location.getCGIPath();
	// for (std::vector<std::string>::iterator it = cgipath.begin(); it != cgipath.end(); ++it)
	// 	if (!it->empty())
	// 		if (lstat(it->c_str(), &st) != 0 || !S_ISREG(st.st_mode) || !(st.st_mode & S_IXUSR))
	// 			throw std::invalid_argument("CGIPath on location: " + location.getPath() + " doesn't exist or is innaccesible");

	std::cout << location.getPath() << std::endl;
	std::vector<std::string> cgiexts = location.getCGIExt();
	for (std::vector<std::string>::iterator it = cgiexts.begin(); it != cgiexts.end(); ++it)
		if (!it->empty())
			if (*it != ".py" && *it != ".sh" && *it != ".php")
				throw std::invalid_argument("CGIExtension on location: " + location.getPath() + " is invalid (only accepts '.py', '.sh', '.php')");
}