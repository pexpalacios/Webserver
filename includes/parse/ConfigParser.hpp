#ifndef CONFIGPARSER
#define CONFIGPARSER

#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <exception>
#include "ServerConfig.hpp"

class ConfigParser 
{
	public:
		ConfigParser();
		ConfigParser(const ConfigParser&);
		ConfigParser &operator=(const ConfigParser&);

		std::vector<ServerConfig> parse(const std::string &filename);
		void checkServerValues(ServerConfig &server);
		void checkLocationValues(LocationConfig &location);
	
		//AUXILIARY FUNCTIONS
		std::vector<std::string> tokenize(const std::string &line);
		std::string omitSpaces(const std::string str);
};

#endif