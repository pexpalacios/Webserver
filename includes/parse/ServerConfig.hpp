#ifndef SERVERCONFIG
#define SERVERCONFIG

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "LocationConfig.hpp"

class ServerConfig
{
	private:
		int _listen;
		size_t _client_max_body_size;

		std::string _host;
		std::string _server_name;
		std::string _error_page;
		std::string _root;
		std::string _index;
		std::vector<LocationConfig> _locations;
	
	public:
		ServerConfig();
		ServerConfig(const ServerConfig&);
		ServerConfig &operator=(const ServerConfig&);
		~ServerConfig();

	//Setters
		void setListen(const int);
		void setClientMaxSize(const size_t);
		void setHost(const std::string);
		void setServerName(const std::string);
		void setErrorPage(const std::string);
		void setRoot(const std::string);
		void setIndex(const std::string);
		void setLocations(const std::vector<LocationConfig>); //maybe this should just receive an obj and add to the vector
		void addLocation(const LocationConfig&);

	//Getters
		int getListen();
		size_t getClientmaxSize();
		std::string getHost();
		std::string getServerName();
		std::string getErrorPage();
		std::string getRoot();
		std::string getIndex();
		std::vector<LocationConfig> getLocations();

	//Print / debug
	void printServer();
};

#endif