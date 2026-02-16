#include "../../includes/parse/ServerConfig.hpp"

ServerConfig::ServerConfig()
	: _listen(0), _client_max_body_size(0), _host(""), _server_name(""), _root(""), _index("")
{
	addLocation(LocationConfig());
}

ServerConfig::ServerConfig(const ServerConfig &copy)
{
	*this = copy;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &copy)
{
	if (this != &copy)
	{
		_listen = copy._listen;
		_client_max_body_size = copy._client_max_body_size;
		_host = copy._host;
		_server_name = copy._server_name;
		_root = copy._root;
		_index = copy._index;
		_error_page = copy._error_page;
		_locations = copy._locations;
	}
	return *this;
}

ServerConfig::~ServerConfig()
{}

////////

void ServerConfig::setListen(const int n)
{
	_listen = n;
}

void ServerConfig::setClientMaxSize(const size_t n)
{
	_client_max_body_size = n;
}

void ServerConfig::setHost(const std::string str)
{
	_host = str;
}

void ServerConfig::setServerName(const std::string str)
{
	_server_name = str;
}

void ServerConfig::setErrorPage(const std::vector<std::string> v)
{
	_error_page = v;
}

void ServerConfig::addErrorPage(const std::string str)
{
	_error_page.push_back(str);
}

void ServerConfig::setRoot(const std::string str)
{
	_root = str;
}

void ServerConfig::setIndex(const std::string str)
{
	_index = str;
}

void ServerConfig::setLocations(const std::vector<LocationConfig> v)
{
	_locations = v;
}

void ServerConfig::addLocation(const LocationConfig &obj)
{
	_locations.push_back(obj);
}

/////////

int ServerConfig::getListen()
{
	return _listen;
}

size_t ServerConfig::getClientmaxSize()
{
	return _client_max_body_size;
}

std::string ServerConfig::getHost()
{
	return _host;
}

std::string ServerConfig::getServerName()
{
	return _server_name;
}

std::vector<std::string> ServerConfig::getErrorPage()
{
	return _error_page;
}

std::string ServerConfig::getRoot()
{
	return _root;
}

std::string ServerConfig::getIndex()
{
	return _index;
}

std::vector<LocationConfig> ServerConfig::getLocations()
{
	return _locations;
}

//////////

void ServerConfig::printServer()
{
	std::cout << "Listen:     " << this->getListen() << std::endl;
	std::cout << "Host:       " << this->getHost() << std::endl;
	std::cout << "ServerName: " << this->getServerName() << std::endl;
	std::cout << "ErrorPage:  ";
	std::vector<std::string> errorPages = this->getErrorPage();
	for (size_t i = 0; i < errorPages.size(); i++)
		std::cout << errorPages[i] << " ";
	std::cout << std::endl;
	std::cout << "Root:       " << this->getRoot() << std::endl;
	std::cout << "Index:      " << this->getIndex() << std::endl;

	std::vector<LocationConfig> locations = this->getLocations();
	for (std::vector<LocationConfig>::iterator i = locations.begin(); i < locations.end(); i++)
		i->printLocation();
}