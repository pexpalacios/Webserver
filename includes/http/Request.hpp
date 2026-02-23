#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request
{
private:
	std::map<std::string, std::string>	headers;
	std::string							method;
	std::string							path;
	std::string							version;
	std::string							body;
	bool								valid;

public:
	Request();
	~Request();

	// ===================== PARSE =====================
	bool	parse(const std::string& raw);
	void	clear();

	// ===================== SETTERS =====================
	void	setMethod(const std::string& m);
	void	setPath(const std::string& p);
	void	setVersion(const std::string& v);
	void	setHeader(const std::string& key, const std::string& value);
	void	setBody(const std::string& b);

	// ===================== GETTERS =====================
	const std::string&	getMethod() const;
	const std::string&	getPath() const;
	const std::string&	getVersion() const;
	const std::string&	getBody() const;

	bool			hasHeader(const std::string& key) const;
	std::string		getHeader(const std::string& key) const;
	const std::map<std::string, std::string>&	getHeaders() const;

	bool			isValid() const;
};

#endif