#ifndef RESPONSE_HANDLER_HPP
#define RESPONSE_HANDLER_HPP

#include "Response.hpp"
#include "../library.hpp"

class ResponseHandler
{
public:
	Response buildStaticFileResponse(const std::string& filePath);
	Response buildErrorResponse(int statusCode);
	Response buildMethodNotAllowed();
};

#endif
