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
		LocationConfig();
		LocationConfig(const LocationConfig&);
		LocationConfig &operator=(const LocationConfig&);
		~LocationConfig();

	//Setters
		void setPath(const std::string);
		void setRoot(const std::string);
		void setIndex(const std::string);
		void setUpload(const std::string);
		void setMethods(const std::vector<std::string>);
		void addMethods(const std::string);
		void setAutoindex(bool);
		void setProtected(bool);

	//Getters
		std::string getPath();
		std::string getRoot();
		std::string getIndex();
		std::string getUpload();
		bool getAutoindex();
		bool getProtected();
		std::vector<std::string> getMethods();
};

#endif