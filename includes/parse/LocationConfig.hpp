#ifndef LOCATIONCONFIG
#define LOCATIONCONFIG

#include "../library.hpp"

//This class stores every possible value a location block can have. It is never instantiatiated
//by itself, it is always a part of a ServerConfig object

class LocationConfig
{
	private:
		std::string _path;
		std::string _root;
		std::string _index;
		std::string _upload;
		std::vector<std::string> _allow_methods;
		std::vector<std::string> _cgi_path;
		std::vector<std::string> _cgi_ext;

		bool _autoindex;
		bool _protected;

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
		void setAutoindex(bool);
		void setProtected(bool);
		void addMethods(const std::string);
		void setMethods(const std::vector<std::string>);
		void addCGIPath(const std::string);
		void setCGIPath(const std::vector<std::string>);
		void addCGIExt(const std::string);
		void setCGIExt(const std::vector<std::string>);

	//Getters
		std::string getPath() const;
		std::string getRoot();
		std::string getIndex();
		std::string getUpload();
		bool getAutoindex();
		bool getProtected();
		std::vector<std::string> getMethods();
		std::vector<std::string> getCGIPath() const;
		std::vector<std::string> getCGIExt() const;

	//Print / debug
	void printLocation();
};

#endif