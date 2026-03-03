#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response
{
private:
	int statusCode;
	std::string statusMessage;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string getStatusMessage(int code) const;

public:
	Response();
	~Response();

	void	setStatusCode(int code);
	int		getStatusCode() const;

	void	setHeader(const std::string& key, const std::string& value);
	void	setBody(const std::string& b);

	std::string toString() const;
};

#endif