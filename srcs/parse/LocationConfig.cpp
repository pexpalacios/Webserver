#include "../../includes/parse/LocationConfig.hpp"

LocationConfig::LocationConfig()
	: _path(""), _root(""), _index(""), _upload(""), _autoindex(false), _protected_(false)
{}

LocationConfig::LocationConfig(const LocationConfig &copy)
{
	*this = copy;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &copy)
{
	if (this != &copy)
	{
		_path = copy._path;
		_root = copy._root;
		_allow_methods = copy._allow_methods;
		_index = copy._index;
		_upload = copy._upload;
		_autoindex = copy._autoindex;
		_protected_ = copy._protected_;
	}
	return *this;
}

LocationConfig::~LocationConfig()
{}

//////////
void LocationConfig::setPath(const std::string str)
{
	_path = str;
}

void LocationConfig::setRoot(const std::string str)
{
	_root = str;
}

void LocationConfig::setIndex(const std::string str)
{
	_index = str;
}

void LocationConfig::setUpload(const std::string str)
{
	_upload = str;
}

void LocationConfig::setMethods(const std::vector<std::string> v)
{
	_allow_methods = v;
}

void LocationConfig::addMethods(const std::string str)
{
	_allow_methods.push_back(str);
}

void LocationConfig::setAutoindex(bool b)
{
	_autoindex = b;
}

void LocationConfig::setProtected(bool b)
{
	_protected_ = b;
}

///////////
std::string LocationConfig::getPath()
{
	return _path;
}

std::string LocationConfig::getRoot()
{
	return _root;
}

std::string LocationConfig::getIndex()
{
	return _index;
}

std::string LocationConfig::getUpload()
{
	return _upload;
}

bool LocationConfig::getAutoindex()
{
	return _autoindex;
}

bool LocationConfig::getProtected()
{
	return _protected_;
}

std::vector<std::string> LocationConfig::getMethods()
{
	return _allow_methods;
}