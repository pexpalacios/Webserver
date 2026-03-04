#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <ctime>
#include <cstdlib>


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

	// Debug log
	std::cout << "[UPDATE] Feed time set to: " << buffer << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Feed time saved\n");
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

	// Debug log
	std::cout << "[UPDATE] Amiwuevo is dead" << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Amiwuevo is dead\n");
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

	// Debug log
	std::cout << "[UPDATE] Name set to: " << name << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Name saved\n");
	return res;
}


//20260304 - Implemented basic POST request handling for a simple background-saving endpoint.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleSetBackground
Response RequestHandler::handleSetBackground(const Request& request) const
{
	std::string body = request.getBody();

	if (body.empty())
		return buildErrorResponse(400);

	std::ofstream file("./database/background.txt");
	if (!file.is_open())
		return buildErrorResponse(500);

	file << body << std::endl;
	file.close();

	// Debug log
	std::cout << "[UPDATE] Background set to: " << body << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Background updated\n");
	return res;
}


//20260304 - Implemented basic POST request handling for a simple clothes-saving endpoint.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleSetClothes
Response RequestHandler::handleSetClothes(const Request& request) const
{
	std::string body = request.getBody();

	if (body.empty())
		return buildErrorResponse(400);

	std::ofstream file("./database/clothes.txt");
	if (!file.is_open())
		return buildErrorResponse(500);

	file << body << std::endl;
	file.close();

	// Debug log
	std::cout << "[UPDATE] Clothes set to: " << body << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Clothes updated\n");
	return res;
}


//20260304 - Implemented basic POST request handling for a simple hope-saving endpoint.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost -> handleSetHope
Response RequestHandler::handleSetHope(const Request& request) const
{
	std::string body = request.getBody();

	if (body.empty())
		return buildErrorResponse(400);

	int hope = std::atoi(body.c_str());

	if (hope < 1 || hope > 100)
		return buildErrorResponse(400);

	std::ofstream file("./database/hope.txt");
	if (!file.is_open())
		return buildErrorResponse(500);

	file << hope << std::endl;
	file.close();

	// Debug log
	std::cout << "[UPDATE] Hope set to: " << hope << std::endl;

	Response res;
	res.setStatusCode(200);
	res.setBody("Hope updated\n");
	return res;
}
