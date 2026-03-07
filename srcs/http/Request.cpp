#include "../../includes/http/Request.hpp"
#include <sstream>
#include <cstdlib>

Request::Request() : valid(false) {}
Request::~Request() {}

// ===================== VALID =====================
bool Request::isValid() const
{return valid;}

// ===================== SETTERS =====================
void Request::setMethod(const std::string& m)
{method = m;}

void Request::setPath(const std::string& p)
{path = p;}

void Request::setVersion(const std::string& v)
{version = v;}

void Request::setHeader(const std::string& key, const std::string& value)
{headers[key] = value;}

void Request::setBody(const std::string& b)
{body = b;}

// ===================== GETTERS =====================
const std::string& Request::getMethod() const
{return method;}

const std::string& Request::getPath() const
{return path;}

const std::string& Request::getVersion() const
{return version;}

const std::string& Request::getBody() const
{return body;}

bool Request::hasHeader(const std::string& key) const
{return headers.find(key) != headers.end();}

std::string Request::getHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(key);

	if (it != headers.end())
		return it->second;
	return "";
}

const std::map<std::string, std::string>& Request::getHeaders() const
{return headers;}


//20260223 Cleanup of parse method
// main -> server.run() -> handleClientConnection() -> Request.parse() -> Request.clear()
void Request::clear()
{
	method.clear();
	path.clear();
	version.clear();
	headers.clear();
	body.clear();
	valid = false;
}


//20260227 URL-decode the path to handle encoded characters (e.g., %20 for space)
// main -> server.run() -> handleClientConnection() -> Request.parse() -> urlDecode()
static std::string urlDecode(const std::string& src)
{
    std::string result;
    size_t i = 0;

    while (i < src.length())
    {
        if (src[i] == '%' && i + 2 < src.length())
        {
            std::string hex = src.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
            result += decodedChar;
            i += 3;
        }
        else
        {
            result += src[i];
            ++i;
        }
    }
    return result;
}


//20260307 Refactor of parse method to handle edge cases and improve readability
// main -> server.run() -> handleClientConnection() -> Request.parse()
bool Request::parse(const std::string& raw)
{
	clear();

	// --- NUEVO ---
	// Buscamos el final de los headers HTTP (\r\n\r\n)
	// Esto separa HEADERS del BODY
	size_t headerEnd = raw.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	// --- NUEVO ---
	// Extraemos solo la parte de los headers
	std::string headerPart = raw.substr(0, headerEnd);

	// --- NUEVO ---
	// El body es todo lo que viene después de los headers
	std::string bodyPart = raw.substr(headerEnd + 4);

	// Ahora el stream solo contiene HEADERS (no binario)
	std::istringstream stream(headerPart);
	std::string line;

	if (!std::getline(stream, line))
		return false;

	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);

	std::istringstream requestLine(line);

	if (!(requestLine >> method >> path >> version))
		return false;

	path = urlDecode(path);

	// 20260223 Parse headers
	while (std::getline(stream, line))
	{
		if (line == "\r" || line == "")
			break;

		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			return false;

		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		if (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		headers[key] = value;
	}

	// 20260223 Parse body if Content-Length is present
	if (hasHeader("Content-Length"))
	{
		int length = std::atoi(getHeader("Content-Length").c_str());
		if (length < 0)
			return false;

		// --- MODIFICADO ---
		// Usamos directamente bodyPart en lugar de leer del stream
		// porque bodyPart puede contener datos binarios (PNG)
		if ((int)bodyPart.size() < length)
			return false;

		body = bodyPart.substr(0, length);
	}

	valid = true;
	return true;
}
