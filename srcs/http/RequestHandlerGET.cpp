#include "../../includes/http/RequestHandler.hpp"
#include <fstream>

//20260225 Terto: Implemented basic POST request handling, including file saving and response generation.
// main -> handleRequest -> handleGet -> resolveGetPath
std::string RequestHandler::resolveGetPath(const std::string& path) const
{
	std::string requestPath = path;
	std::string root = _server.getStaticRoot();
	std::string index = _server.getIndexFile();

	const LocationConfig* locPtr = findMatchingLocation(path);

	// if there's a matching location, we need to adjust the root and index based on that location's config
	if (locPtr)
	{
		// Local copy to avoid const issues
		LocationConfig loc = *locPtr;

		std::string locPath = loc.getPath();

		if (!loc.getRoot().empty())
			root = loc.getRoot();

		if (requestPath.find(locPath) == 0)
			requestPath = requestPath.substr(locPath.length());

		if (requestPath.empty() || requestPath == "/")
		{
			if (!loc.getIndex().empty())
				requestPath = loc.getIndex();
			else
				requestPath = index;
		}
	}
	else
	{
		if (requestPath == "/")
			requestPath = index;
	}

	if (requestPath.find(root) == 0)
		return requestPath;

	if (!root.empty() && root[root.size() - 1] != '/')
		root += '/';

	if (!requestPath.empty() && requestPath[0] == '/')
		requestPath.erase(0, 1);

	return root + requestPath;
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

//20260223 - build a successful response with the file content and appropriate headers
// main -> handleRequest -> handleGet -> buildFileResponse
Response RequestHandler::buildFileResponse(const std::string& content, const std::string& filePath) const
{
	Response response;
	response.setStatusCode(200);
	response.setHeader("Content-Type", getContentType(filePath));
	response.setBody(content);

	return (response);
}

