#ifndef LOCATIONCONFIG
#define LOCATIONCONFIG

#include <iostream>
#include <vector>

class LocationConfig
{
	private:
		std::string _path;
		std::string _root;
		std::vector<std::string> _allow_methods;
		std::string _index;
		std::string _upload;

		bool _autoindex;
		bool _protected_;
	public:
	//Setters
	//Getters
};

#endif