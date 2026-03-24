#ifndef SERVERCONFIG
#define SERVERCONFIG

#include "../library.hpp"
#include "LocationConfig.hpp"

//This class stores every value a server block can have

class ServerConfig
{
	private:
		std::vector<int> _listen;
		size_t _client_max_body_size;
		std::string _host;
		std::string _server_name;
		std::string _root;
		std::string _index;
		std::vector<std::string> _error_page;
		std::vector<LocationConfig> _locations;
	
	public:
		ServerConfig();
		ServerConfig(const ServerConfig&);
		ServerConfig &operator=(const ServerConfig&);
		~ServerConfig();

	//Setters
		void addListen(const int);
		void setListen(const std::vector<int>);
		void setClientMaxSize(const size_t);
		void setHost(const std::string);
		void setServerName(const std::string);
		void setRoot(const std::string);
		void setIndex(const std::string);
		void addErrorPage(const std::string);
		void setErrorPage(const std::vector<std::string>);
		void setLocations(const std::vector<LocationConfig>); //maybe this should just receive an obj and add to the vector
		void addLocation(const LocationConfig&);

	//Getters
		std::vector<int> getListen();
		size_t getClientMaxSize();
		std::string getHost();
		std::string getServerName();
		std::vector<std::string> getErrorPage();
		std::string getRoot();
		std::string getIndex();
		std::vector<LocationConfig> getLocations();

	//Print / debug
	void printServer();
};

#endif