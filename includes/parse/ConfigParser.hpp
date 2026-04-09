#ifndef CONFIGPARSER
#define CONFIGPARSER

#include "../library.hpp"
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
		std::string omitSpaces(const std::string &str);
	private:
		void checkIpPortPairs(std::vector<ServerConfig> &servers);
		// 20260325 Alex: small struct to check for reapeated ip:port server_name
		// Believe me, it was this or use a very ugly std::set<std::pair<std::pair<std::string, int>, std::string> > seen_pairs;
		struct ListenKey
		{
			std::string host;
			int port;
			std::string name;

			bool operator<(const ListenKey &other) const;
		};
		
};

#endif