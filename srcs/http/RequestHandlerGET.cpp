#include "../../includes/http/RequestHandler.hpp"
#include <fstream>

//20260223 - resolve the requested path to a file system path, ensuring it points to the www directory
// main -> handleRequest -> handleGet -> resolveGetPath
std::string RequestHandler::resolveGetPath(const std::string& path) const
{
	std::string finalPath = path;

	// Si es "/", usamos index
	if (path == "/")
		finalPath = _server.getIndexFile();
	std::string root = _server.getStaticRoot();

	// Asegurar que root termina en '/'
	if (!root.empty() && root[root.size() - 1] != '/')
		root += '/';

	// Si finalPath empieza con '/', quitarlo
	if (!finalPath.empty() && finalPath[0] == '/')
		finalPath.erase(0, 1);

	return root + finalPath;
}

//20260223 - read the content of the file into a string, return empty string if file cannot be read or have no permission
// main -> handleRequest -> handleGet -> readFileContent
std::string RequestHandler::readFileContent(const std::string& path) const
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
		return "";

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return content;
}

//20260223 - determine the content type based on file extension
// main -> handleRequest -> handleGet -> getContentType
std::string RequestHandler::getContentType(const std::string& path) const
{
	if (path.find(".html") != std::string::npos)
		return ("text/html");
	if (path.find(".css") != std::string::npos)
		return ("text/css");
	if (path.find(".js") != std::string::npos)
		return ("application/javascript");
	if (path.find(".png") != std::string::npos)
		return ("image/png");
	if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
		return ("image/jpeg");
	if (path.find(".gif") != std::string::npos)
		return ("image/gif");

	return ("text/plain");
}

Response RequestHandler::buildFileResponse(const std::string& content, const std::string& filePath) const
{
	Response response;
	response.setStatusCode(200);
	response.setHeader("Content-Type", getContentType(filePath));
	response.setBody(content);

	return (response);
}