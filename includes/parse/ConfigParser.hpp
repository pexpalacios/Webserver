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

//This class parses a config file to store everything into a ServerConfig object.
//It tokenizes values and makes sure they are valid

class ConfigParser 
{
	public:
		ConfigParser();
		ConfigParser(const ConfigParser&);
		ConfigParser &operator=(const ConfigParser&);
		~ConfigParser();

		// MAIN FUNCTIONS
		std::vector<ServerConfig> parse(const std::string &filename);
		void checkServerValues(ServerConfig &server);
		void checkLocationValues(LocationConfig &location, const std::string);
	
		//AUXILIARY FUNCTIONS
		std::vector<std::string> tokenize(const std::string &line);
		std::string omitSpaces(const std::string str);
};

#endif