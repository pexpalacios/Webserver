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

//20260309 - ALEX: Implemented basic GET request handling for dialog
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getDialog
Response RequestHandler::getDialog() const
{
	static size_t line_count = 0;
	static std::vector<std::string> dialog;

	// In first read, loadas the dialog string vector;
	if (dialog.empty()){
		std::ifstream file("./database/text.txt");
		if (!file.is_open())
			return buildErrorResponse(404);
		std::string buffer;
		while (std::getline(file, buffer))
			dialog.push_back(buffer);
		file.close();
	}
	if (dialog.empty())
		return buildErrorResponse(500);

	// After loading the 
	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/plain");
	if (line_count < dialog.size())
		res.setBody(dialog[line_count]);
	else {
		line_count = 0;
		res.setBody(dialog[line_count]);
	}
	logGetRequest("database/text.txt");
	line_count++;
	return res;
}