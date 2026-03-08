#include "../../includes/http/RequestHandler.hpp"
#include <fstream>

//20260304 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getName
Response RequestHandler::getName() const
{
	std::ifstream file("./database/name.txt");
	if (!file.is_open())
		return buildErrorResponse(404);

	std::string name;
	std::getline(file, name);
	file.close();

	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/plain");
	res.setBody(name);
	return res;
}


//20260304 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getBackground
Response RequestHandler::getBackground() const
{
	std::ifstream file("./database/background.txt");
	if (!file.is_open())
		return buildErrorResponse(404);

	std::string path;
	std::getline(file, path);
	file.close();

	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/plain");
	res.setBody(path);
	return res;
}


//20260304 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getClothes
Response RequestHandler::getClothes() const
{
	std::ifstream file("./database/clothes.txt");
	if (!file.is_open())
		return buildErrorResponse(404);

	std::string path;
	std::getline(file, path);
	file.close();

	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/plain");
	res.setBody(path);
	return res;
}


//20260304 - Implemented basic GET request handling, including file reading and response generation.
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getHope
Response RequestHandler::getHope() const
{
	std::ifstream file("./database/hope.txt");
	if (!file.is_open())
		return buildErrorResponse(404);

	std::string value;
	std::getline(file, value);
	file.close();

	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/plain");
	res.setBody(value);
	return res;
}