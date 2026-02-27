#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

RequestHandler::RequestHandler(const Server& server)
: _server(server) {}

Response RequestHandler::handleBadRequest()
{return buildErrorResponse(400);}

Response RequestHandler::handleInternalServerError()
{return buildErrorResponse(500);}


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


//20260226 - Build error response with safe fallback
// Used for all HTTP error codes (400, 403, 404, 500, etc.)
// main -> server.run() -> handleClientConnection() -> handleRequest -> buildErrorResponse
#include <sstream>  // 👈 importante

Response RequestHandler::buildErrorResponse(int statusCode) const
{
    Response response;
    response.setStatusCode(statusCode);
    response.setHeader("Content-Type", "text/html");

    std::map<int, std::string> errorPages = _server.getErrorPages();
    std::string body;

    // Convert int to string (C++98 compatible)
    std::ostringstream oss;
    oss << statusCode;
    std::string codeStr = oss.str();

    if (errorPages.find(statusCode) != errorPages.end())
    {
        std::string filePath = errorPages.find(statusCode)->second;

        try
        {
            body = readFileContent(filePath);
        }
        catch (...)
        {
            body = "<html><head><title>" + codeStr + " Error</title></head>"
                   "<body><h1>" + codeStr + " - Internal Server Error</h1>"
                   "<p>The server encountered an unexpected condition.</p>"
                   "</body></html>";
        }
    }
    else
    {
        body = "<html><head><title>" + codeStr + " Error</title></head>"
               "<body><h1>Error " + codeStr + "</h1>"
               "</body></html>";
    }

    response.setBody(body);
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


//20260226 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handleGet(const Request& request)
{
	std::string path = request.getPath();

	// 20260226 - Manual 500 trigger for testing
	if (path == "/trigger500")
		return buildErrorResponse(500);

	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveGetPath(path);
	std::string root = _server.getStaticRoot();
	if (filePath.find(root) != 0)
		return buildErrorResponse(403);

	if (!fileExists(filePath))
		return buildErrorResponse(404);

	// 20260225 Debug method
	DebugHandleGet(path, filePath);

	std::string content = readFileContent(filePath);
	return buildFileResponse(content, filePath);
}


// 20260223 - Basic method for not allowed response
// main -> handleRequest -> methodNotAllowed
Response RequestHandler::methodNotAllowed()
{
	//return buildErrorResponse(405);
	Response response;
	response.setStatusCode(405);
	response.setHeader("Content-Type", "text/plain; charset=utf-8");
	response.setHeader("Allow", "GET, POST, DELETE");
	response.setBody("405 Method Not Allowed\nThis endpoint does not accept that HTTP method.\n");
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
