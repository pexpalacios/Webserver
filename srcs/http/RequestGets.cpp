#include "../../includes/http/RequestHandler.hpp"
#include <fstream>
#include <sys/stat.h>

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


//20260309 - Terto: Implemented basic GET request handling, including file reading and response generation.
bool RequestHandler::fileExists(const std::string& path) const
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

//20260309 - GET uploaded background
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getUploadedBackground
Response RequestHandler::getUploadedBackground() const
{
	std::ifstream file("./database/bg.txt");

	if (!file.is_open())
		return buildErrorResponse(404);

	std::string path;
	std::getline(file, path);
	file.close();

	Response res;

	if (path.empty())
	{
		res.setStatusCode(204);
		return res;
	}

	std::string filePath = "./www/amiwuevo" + path;
	if (!fileExists(filePath))
	{
		res.setStatusCode(204);
		return res;
	}

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

//20260309 - Get current uploaded background filename from database/bg.txt
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getCurrentBackgroundFileName
std::string RequestHandler::getCurrentBackgroundFileName() const
{
	std::ifstream file("./database/bg.txt");

	if (!file.is_open())
		return "";

	std::string name;
	std::getline(file, name);
	file.close();

	return name;
}

//20260309 - Generate next background filename (bg_0001.png, bg_0002.png...)
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> getNextBackgroundFileName
// Reads current value from database/bg.txt and increments the number
std::string RequestHandler::getNextBackgroundFileName() const
{
	std::ifstream file("./database/bg.txt");

	std::string current;
	int number = 0;

	if (file.is_open())
	{
		std::getline(file, current);
		file.close();
	}

	if (!current.empty() && current.find("bg_") == 0)
	{
		size_t start = current.find('_');
		size_t end = current.find('.');

		if (start != std::string::npos && end != std::string::npos && end > start + 1)
		{
			std::string numStr = current.substr(start + 1, end - start - 1);
			number = std::atoi(numStr.c_str());
		}
	}

	number++;

	std::ostringstream name;
	name << "bg_";

	if (number < 10)
		name << "000";
	else if (number < 100)
		name << "00";
	else if (number < 1000)
		name << "0";

	name << number << ".png";

	return name.str();
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