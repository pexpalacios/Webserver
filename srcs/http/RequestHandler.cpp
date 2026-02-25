#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

//20260223 - Implemented basic GET request handling, including file reading and response generation.
RequestHandler::RequestHandler(const Server& server)
: _server(server) {}

Response RequestHandler::handleBadRequest()
{return buildErrorResponse(400);}


//20260223 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete -> isPathSafe
bool RequestHandler::isPathSafe(const std::string& path) const
{
	if (path.empty())
		return false;

	if (path[0] != '/')
		return false;

	if (path.find("..") != std::string::npos)
		return false;

	if (path.find("~") != std::string::npos)
		return false;

	return true;
}


//20260223 - resolve the requested path to a file system path, not allowing directory traversal and ensuring it points to the www directory
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete -> fileExists
bool RequestHandler::fileExists(const std::string& path) const
{
	struct stat buffer;

	if (stat(path.c_str(), &buffer) != 0)
		return false;

	return S_ISREG(buffer.st_mode);
}


//20260223 - read the content of the file into a string
// main -> handleRequest -> handleGet -> readFileContent
Response RequestHandler::buildErrorResponse(int statusCode) const
{
	Response response;
	response.setStatusCode(statusCode);

	std::map<int, std::string> errorPages = _server.getErrorPages();

	if (errorPages.find(statusCode) != errorPages.end())
	{
		std::string filePath = errorPages.find(statusCode)->second;
		std::string content = readFileContent(filePath);

		response.setBody(content);
		response.setHeader("Content-Type", "text/html");
	}
	else
	{
		response.setBody("<h1>Error</h1>");
		response.setHeader("Content-Type", "text/html");
	}

	return response;
}


//20260225 find the best matching location for the requested path, based on longest prefix match
// main -> handleRequest -> handleGet -> findMatchingLocation
const LocationConfig* RequestHandler::findMatchingLocation(const std::string& path) const
{
	const std::vector<LocationConfig>& locations = _server.getLocations();

	const LocationConfig* matched = NULL;
	size_t longestMatch = 0;
	size_t i = 0;

	while (i < locations.size())
	{
		// local copy to avoid getter const problems
		LocationConfig loc = locations[i];

		std::string locPath = loc.getPath();

		if (!locPath.empty() &&
			path.find(locPath) == 0 &&
			locPath.length() > longestMatch)
		{
			matched = &locations[i];
			longestMatch = locPath.length();
		}

		++i;
	}

	return (matched);
}


//20260223 - switched to route requests based on HTTP method
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handleDelete(const Request& request)
{
	std::string path = request.getPath();
	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveDeletePath(path);
	if (!fileExists(filePath))
		return buildErrorResponse(404);

	if (!deleteFile(filePath))
		return buildErrorResponse(500);

	return buildNoContentResponse();
}


//20260223 - Implemented basic POST request handling
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handlePost(const Request& request)
{
	std::string path = request.getPath();
	if (!isPathSafe(path))
		return buildErrorResponse(403);

	if (request.getBody().empty())
		return buildErrorResponse(400);

	std::string filePath = resolvePostPath(path);
	if (!saveUploadedFile(filePath, request.getBody()))
		return buildErrorResponse(500);

	return buildCreatedResponse();
}


//20260223 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handleGet(const Request& request)
{
	std::string path = request.getPath();
	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveGetPath(path);
	std::string root = _server.getStaticRoot();
	if (filePath.find(root) != 0)
		return buildErrorResponse(403);

	// Debug method to print the requested path and the resolved file path for GET requests
	DebugHandleGet(path, filePath);

	if (!fileExists(filePath))
		return buildErrorResponse(404);

	std::string content = readFileContent(filePath);
	return buildFileResponse(content, filePath);
}


// 20260223 - Basic method for not allowed response
// main -> handleRequest -> methodNotAllowed
Response RequestHandler::methodNotAllowed()
{
	Response response;
	response.setStatusCode(405);
	response.setHeader("Content-Type", "text/html");
	response.setBody("<h1>405 Method Not Allowed</h1>");

	return response;
}


//20260223 - switched to route requests based on HTTP method
// main -> server.run() -> handleClientConnection() -> handleRequest
Response RequestHandler::handleRequest(const Request& request)
{
	if (request.getMethod() == "GET")
		return handleGet(request);
	else if (request.getMethod() == "POST")
		return handlePost(request);
	else if (request.getMethod() == "DELETE")
		return handleDelete(request);
	else
		return methodNotAllowed();
}
