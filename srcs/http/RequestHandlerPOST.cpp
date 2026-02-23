#include "../../includes/http/RequestHandler.hpp"
#include <fstream>

//20260223 - resolve the actual file path for POST requests
// main -> handleRequest -> handlePost -> resolvePostPath
std::string RequestHandler::resolvePostPath(const std::string& path) const
{
	std::string root = "./www";

	if (path == "/")
		return root + "/upload.txt";

	return root + path;
}

//20260223 - save the uploaded content to a file on the server
// main -> handleRequest -> handlePost -> saveUploadedFile
bool RequestHandler::saveUploadedFile(const std::string& path, const std::string& body) const
{
	std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);

	if (!file.is_open())
		return false;

	file << body;
	file.close();

	return true;
}

//20260223 - build a 201 Created response for successful POST requests
// main -> handleRequest -> handlePost -> buildCreatedResponse
Response RequestHandler::buildCreatedResponse() const
{
	Response response;
	response.setStatusCode(201);
	response.setBody("Created");
	response.setHeader("Content-Type", "text/plain");
	return response;
}