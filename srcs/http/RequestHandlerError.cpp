#include "../../includes/http/RequestHandler.hpp"

// 20260303 - Build error response with 400 status code for bad requests
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete -> handleBadRequest
Response RequestHandler::handleBadRequest()
{return buildErrorResponse(400);}

// 20260223 - Basic method for not allowed response
// main -> handleRequest -> methodNotAllowed
Response RequestHandler::methodNotAllowed()
{return buildErrorResponse(405);}

// 20260303 - Internal server error response builder, used for 500 errors and as a fallback for other error responses
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete -> handleInternalServerError
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

//20260303 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete -> checkFile
int RequestHandler::checkFile(const std::string& path) const
{
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
	{
	std::cout << "stat failed for: " << path << std::endl;
	std::cout << "errno value: " << errno << std::endl;

		if (errno == ENOENT) // No such file or directory
			return (404);
		else if (errno == EACCES) // Permission denied
			return (403);
		else
			return (500);
	}

	if (!S_ISREG(st.st_mode))
		return (403); // Forbidden - Not a regular file

	if (access(path.c_str(), R_OK) == -1)
	{
		if (errno == EACCES)
			return (403); // Forbidden - No read permission
		return (500); // Internal Server Error - Other access error
	}
	return (200); // OK
}


//20260226 - Build error response with safe fallback. Used for all HTTP error codes (400, 403, 404, 500, etc.)
// main -> server.run() -> handleClientConnection() -> handleRequest -> buildErrorResponse
Response RequestHandler::buildErrorResponse(int statusCode) const
{
	Response response;
	response.setStatusCode(statusCode);
	response.setHeader("Content-Type", "text/html");

	std::map<int, std::string> errorPages = _server.getErrorPages();
	std::string body;

	std::ostringstream oss;
	oss << statusCode;
	std::string codeStr = oss.str();

	if (errorPages.find(statusCode) != errorPages.end())
	{
		std::string filePath = errorPages.find(statusCode)->second;
		try
		{
			body = readFileContent(filePath);
			if (body.empty())
				throw std::runtime_error("Error page file is empty or not found");
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
