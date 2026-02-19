#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{
public:
    Response handleRequest(const Request& request);

private:
    Response handleGet(const Request& request);
    Response handlePost(const Request& request);
    Response handleDelete(const Request& request);
    Response methodNotAllowed();
};

#endif
