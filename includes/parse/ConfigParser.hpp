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
	
		//check file exists and isn't empty and can be opened
		//check extension of file
};

#endif