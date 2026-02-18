#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "../Server.hpp"


class Request
{
public:
	Request();
	~Request();

	std::string	method;
	std::string	path;
	std::string	version;
	std::string	body;
	std::map<std::string, std::string>	headers;
}

#endif