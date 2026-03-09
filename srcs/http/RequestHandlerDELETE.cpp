#include "../../includes/http/RequestHandler.hpp"
#include <unistd.h>

//20260223 - resolve the actual file path for DELETE requests, similar to GET but can be customized if needed
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleDelete -> resolveDeletePath
std::string RequestHandler::resolveDeletePath(const std::string& path) const
{
	std::string root = "./www/amiwuevo";
	return root + path;
}

//20260223 - delete the specified file from the filesystem
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleDelete -> deleteFile
bool RequestHandler::deleteFile(const std::string& path) const
{
	return (unlink(path.c_str()) == 0);
}

//20260223 - build a 204 No Content response for successful DELETE requests
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleDelete -> buildNoContentResponse
Response RequestHandler::buildNoContentResponse() const
{
	Response response;
	response.setStatusCode(204);
	return response;
}


//20260310 - Handle DELETE request for uploaded background
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleDelete -> handleDeleteUploadedBackground
Response RequestHandler::handleDeleteUploadedBackground() const
{
	std::string bgPath = getCurrentBackgroundFileName();

	if (bgPath.empty())
		return buildErrorResponse(404);

	std::string filePath = "./www/amiwuevo" + bgPath;

	if (!deleteFile(filePath))
		return buildErrorResponse(500);

	std::cout << "[DELETE] background removed: " << filePath << std::endl;

	return buildNoContentResponse();
}