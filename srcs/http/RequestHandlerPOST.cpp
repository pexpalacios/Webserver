#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <ctime>


//20260223 - handle POST requests by saving the body content to a file and returning a response
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleFeed
Response RequestHandler::handleFeed() const
{
	std::ofstream out("./database/feed.txt");
	if (!out.is_open())
		return buildErrorResponse(500);

	std::time_t now = std::time(NULL);
	std::tm* local = std::localtime(&now);

	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);

	out << buffer << std::endl;
	out.close();

	Response res;
	res.setStatusCode(200);
	res.setBody("Feed time saved");
	return res;
}


//20260223 - handle POST requests by saving the body content to a file and returning a response
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleKill
Response RequestHandler::handleKill() const
{
	std::ofstream out("./database/alive.txt");
	if (!out.is_open())
		return buildErrorResponse(500);

	out << "0" << std::endl;
	out.close();

	Response res;
	res.setStatusCode(200);
	res.setBody("Amiwuevo is dead");
	return res;
}


//20260303 - Implemented basic POST request handling for a simple name-saving endpoint.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleSetName
Response RequestHandler::handleSetName(const Request& request)
{
	std::string name = request.getBody();

	if (name.empty() || name.length() > 30)
		return buildErrorResponse(400);

	std::ofstream file("./database/name.txt");
	if (!file.is_open())
		return buildErrorResponse(500);

	file << name << std::endl;
	file.close();

	Response res;
	res.setStatusCode(200);
	res.setBody("Name saved");
	return res;
}