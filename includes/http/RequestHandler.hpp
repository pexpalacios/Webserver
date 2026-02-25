#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "../Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>

// Forward declaration to avoid circular dependency
class Server;

class RequestHandler
{
public:
	RequestHandler(const Server& server);
	Response	handleRequest(const Request& request);
	Response	handleBadRequest();

private:
	const Server& _server;

	Response	handleGet(const Request& request);
	Response	handlePost(const Request& request);
	Response	handleDelete(const Request& request);
	Response	methodNotAllowed();

	// Helpers for all methods
	bool		isPathSafe(const std::string& path) const;
	bool		fileExists(const std::string& path) const;
	Response	buildErrorResponse(int statusCode) const;

	// Location matching
	const LocationConfig*	findMatchingLocation(const std::string& path) const;

	// GET
	std::string	resolveGetPath(const std::string& path) const;
	std::string	readFileContent(const std::string& path) const;
	std::string	getContentType(const std::string& path) const;
	Response	buildFileResponse(const std::string& content, const std::string& filePath) const;
	void		DebugHandleGet(const std::string& path, const std::string& resolvedPath) const;

	// POST
	std::string	resolvePostPath(const std::string& path) const;
	bool		saveUploadedFile(const std::string& path, const std::string& body) const;
	Response	buildCreatedResponse() const;

	// DELETE
	std::string	resolveDeletePath(const std::string& path) const;
	bool		deleteFile(const std::string& path) const;
	Response	buildNoContentResponse() const;
};


#endif
