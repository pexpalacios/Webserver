#include "../../includes/Server.hpp"
#include "../../includes/http/Response.hpp"
#include <sstream>

Response::Response()
{
	statusCode = 200;
	statusMessage = "OK";
}
Response::~Response() {}

int Response::getStatusCode() const
{return (statusCode);}


// ======================= SETTERS ===========================
void Response::setStatusCode(int code)
{
	statusCode = code;
	statusMessage = getStatusMessage(code);
}

void Response::setHeader(const std::string& key, const std::string& value)
{headers[key] = value;}

void Response::setBody(const std::string& b)
{body = b;}


std::string Response::toString() const
{
	std::ostringstream response;

	response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
	std::map<std::string, std::string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		response << it->first << ": " << it->second << "\r\n";	
			++it;
	}
	response << "Content-Length: " << body.length() << "\r\n";
	response << "Connection: close\r\n";
	response << "\r\n";
	response << body;

	return response.str();
}

std::string Response::getStatusMessage(int code) const
{
	if (code == 200) return "OK";
	if (code == 201) return "Created";
	if (code == 204) return "No Content";
	if (code == 400) return "Bad Request";
	if (code == 403) return "Forbidden";
	if (code == 404) return "Not Found";
	if (code == 405) return "Method Not Allowed";
	if (code == 500) return "Internal Server Error";
	return "Error";
}