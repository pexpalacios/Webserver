#include "../../includes/http/RequestHandler.hpp"
#include <unistd.h>

//20260223 - resolve the actual file path for DELETE requests, similar to GET but can be customized if needed
// main -> handleRequest -> handleDelete -> resolveDeletePath
std::string RequestHandler::resolveDeletePath(const std::string& path) const
{
	std::string root = "./www/amiwuevo";
	return root + path;
}

//20260223 - delete the specified file from the filesystem
// main -> handleRequest -> handleDelete -> deleteFile
bool RequestHandler::deleteFile(const std::string& path) const
{
	return (unlink(path.c_str()) == 0);
}

//20260223 - build a 204 No Content response for successful DELETE requests
// main -> handleRequest -> handleDelete -> buildNoContentResponse
Response RequestHandler::buildNoContentResponse() const
{
	Response response;
	response.setStatusCode(204);
	return response;
}