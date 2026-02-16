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

static int findPage(std::string name, std::string root)
{	
	if (name.length() < 5 || name.substr(name.length() - 5) != ".html")
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
	if (!(server.getListen() >= 1 && server.getListen() <= 65535))
		std::invalid_argument("Listen in server: " + server.getServerName() + " is out of listening range");
	if (!isValidIPv4(server.getHost()))
		std::invalid_argument("Host in server: " + server.getServerName() + " is not a valid IPv4 value");

	if (!findServer(server.getServerName()))
		std::invalid_argument("Server: " + server.getServerName() + " does not exist or is innaccesible");

	std::vector<std::string> errorPages = server.getErrorPage();
	for (size_t i = 0; i < errorPages.size(); i++)
	{
		if (!errorPages[i].empty() && !findPage(errorPages[i], server.getRoot()))
			std::invalid_argument("Page: " + errorPages[i] + " does not exist in Server: " + server.getServerName());
	}

	if (server.getClientmaxSize() == 0)
		std::invalid_argument("Client max body size in server: " + server.getServerName() + " is zero");
	if (server.getClientmaxSize() > 1073741824) // 1GB max
		std::invalid_argument("Client max body size in server: " + server.getServerName() + " exceeds maximum (1GB)");

	struct stat st;
	if (lstat(server.getRoot().c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		std::invalid_argument("Root directory: " + server.getRoot() + " does not exist or is not a directory");
	if (!findPage(server.getIndex(), server.getRoot()))
		std::invalid_argument("Index page: " + server.getIndex() + " does not exist");
}