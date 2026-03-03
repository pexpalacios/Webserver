#include "../../includes/http/RequestHandler.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

RequestHandler::RequestHandler(const Server& server)
: _server(server) {}

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
	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	if (!deleteFile(filePath))
		return buildErrorResponse(500);

	return buildNoContentResponse();
}


//20260223 - Implemented basic POST request handling
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handlePost(const Request& request)
{
	std::string path = request.getPath();

	if (path == "/api/name")
		return handleSetName(request);

	if (path == "/api/kill")
		return handleKill();

	if (path == "/api/feed")
		return handleFeed();

	return buildErrorResponse(404);
}

void RequestHandler::logGetRequest(const std::string& path) const
{
	static int getCounter = 0;  // ← vive solo aquí
	getCounter++;

	std::cout << "[GET #" << getCounter << "] " << path << std::endl;
}


//20260303 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handleGet(const Request& request)
{
	std::string path = request.getPath();

	if (path == "/trigger500")
		return buildErrorResponse(500);

	if (!isPathSafe(path))
		return buildErrorResponse(403); // Forbidden

	std::string filePath = resolveGetPath(path);
	std::string root = _server.getStaticRoot();
	if (filePath.find(root) != 0)
		return buildErrorResponse(403); // Forbidden

	std::string resolvedPath = filePath;
	logGetRequest(resolvedPath); // Log the resolved path for debugging

	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	std::string content = readFileContent(filePath);
	return buildFileResponse(content, filePath);
}


// 20260223 - Basic method for not allowed response
// main -> handleRequest -> methodNotAllowed
Response RequestHandler::methodNotAllowed()
{
	return buildErrorResponse(405);	// Method Not Allowed
	/*
	Response response;
	response.setStatusCode(405);
	response.setHeader("Content-Type", "text/plain; charset=utf-8");
	response.setHeader("Allow", "GET, POST, DELETE");
	response.setBody("405 Method Not Allowed\nThis endpoint does not accept that HTTP method.\n");
	return response;
	*/
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
		return methodNotAllowed(); // Protection 405 → Method Not Allowed
}
