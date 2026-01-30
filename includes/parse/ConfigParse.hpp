#ifndef CONFIGPARSER
#define CONFIGPARSER

#include <algorithm>
#include <vector>
#include <iostream>
#include "ServerConfig.hpp"

class ConfigParser 
{
	public:
		std::vector<ServerConfig> parse(const std::string &filename);
};

#endif