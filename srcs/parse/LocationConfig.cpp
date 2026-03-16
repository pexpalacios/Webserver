#include "../../includes/parse/LocationConfig.hpp"

/// CONSTRUCTORS AND DESTRUCTORS
LocationConfig::LocationConfig()
	: _path(""), _root(""), _index(""), _upload(""), _autoindex(false), _protected(false)
{
}

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
		_protected = copy._protected;
		_cgi_path = copy._cgi_path;
		_cgi_ext = copy._cgi_ext;
	}
	return *this;
}

LocationConfig::~LocationConfig()
{
}

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

void LocationConfig::addMethods(const std::string str)
{
	_allow_methods.push_back(str);
}

void LocationConfig::setMethods(const std::vector<std::string> v)
{
	_allow_methods = v;
}

void LocationConfig::addCGIPath(const std::string str)
{
	_cgi_path.push_back(str);
}

void LocationConfig::setCGIPath(const std::vector<std::string> v)
{
	_cgi_path = v;
}

void LocationConfig::addCGIExt(const std::string str)
{
	_cgi_ext.push_back(str);
}

void LocationConfig::setCGIExt(const std::vector<std::string> v)
{
	_cgi_ext = v;
}

void LocationConfig::setAutoindex(bool b)
{
	_autoindex = b;
}

void LocationConfig::setProtected(bool b)
{
	_protected = b;
}

///////////

std::string LocationConfig::getPath() const
{
	return (_path);
}

std::string LocationConfig::getRoot()
{
	return (_root);
}

std::string LocationConfig::getIndex()
{
	return (_index);
}

std::string LocationConfig::getUpload()
{
	return (_upload);
}

bool LocationConfig::getAutoindex()
{
	return (_autoindex);
}

bool LocationConfig::getProtected()
{
	return (_protected);
}

std::vector<std::string> LocationConfig::getMethods()
{
	return (_allow_methods);
}

std::vector<std::string> LocationConfig::getCGIPath() const
{
	return (_cgi_path);
}

std::vector<std::string> LocationConfig::getCGIExt() const
{
	return (_cgi_ext);
}

////////////

void LocationConfig::printLocation()
{
	std::cout << "Path:      " << this->getPath() << std::endl;
	std::cout << "Root:      " << this->getRoot() << std::endl;
	std::cout << "Index:     " << this->getIndex() << std::endl;
	std::cout << "Upload:    " << this->getUpload() << std::endl;
	std::cout << "Autoindex: " << this->getAutoindex() << std::endl;
	std::cout << "Protected: " << this->getProtected() << std::endl;

	std::vector<std::string> methods = this->getMethods();
	for (std::vector<std::string>::iterator i = methods.begin(); i < methods.end(); i++)
		std::cout << "Method: " << i->c_str() << std::endl;
}