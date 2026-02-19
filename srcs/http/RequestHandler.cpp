#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <unistd.h>


Response RequestHandler::handleGet(const Request& request)
{
    Response response;
    std::string filePath = "./www" + request.getPath();

    std::ifstream file(filePath.c_str());
    if (!file.is_open())
    {
        response.setStatus(404);
        response.setBody("<h1>404 Not Found</h1>");
        response.setHeader("Content-Type", "text/html");
        return response;
    }

    // Read file content
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    response.setStatus(200);
    response.setBody(content);
    response.setHeader("Content-Type", "text/html");

    return response;
}


Response RequestHandler::methodNotAllowed()
{
    Response response;
    response.setStatus(405);
    response.setBody("<h1>405 Method Not Allowed</h1>");
    response.setHeader("Content-Type", "text/html");
    return response;
}


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
