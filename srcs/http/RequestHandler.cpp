#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../../includes/http/RequestHandler.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

RequestHandler::RequestHandler(const Server &server)
	: _server(server) {}

// 20260225 find the best matching location for the requested path, based on longest prefix match
//  main -> handleRequest -> handleGet -> findMatchingLocation
const LocationConfig *RequestHandler::findMatchingLocation(const std::string &path) const
{
	const std::vector<LocationConfig> &locations = _server.getLocations();

	const LocationConfig *matched = NULL;
	size_t longestMatch = 0;
	size_t i = 0;

	while (i < locations.size())
	{
		// local copy to avoid getter const problems
		LocationConfig loc = locations[i];

		std::string locPath = loc.getPath();

		if (!locPath.empty() &&
			path.find(locPath) == 0 &&
			locPath.length() > longestMatch)
		{
			matched = &locations[i];
			longestMatch = locPath.length();
		}

		++i;
	}
	return (matched);
}

// 20260303 - main request handling method that routes to specific handlers based on HTTP method
//  html -> handleGetName
Response RequestHandler::GetName() const
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

// 20260223 - switched to route requests based on HTTP method
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handleDelete(const Request &request)
{
	std::string path = request.getPath();
	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveDeletePath(path);
	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	if (!deleteFile(filePath))
		return buildErrorResponse(500);

	return buildNoContentResponse();
}

// 20260304 - Implemented basic POST request handling
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
Response RequestHandler::handlePost(const Request &request)
{
	std::string path = request.getPath();

	if (path == "/api/name")
		return handleSetName(request);

	if (path == "/api/background")
		return handleSetBackground(request);

	if (path == "/api/clothes")
		return handleSetClothes(request);

	if (path == "/api/hope")
		return handleSetHope(request);

	if (path == "/api/kill")
		return handleKill();

	if (path == "/api/feed")
		return handleFeed();

	return buildErrorResponse(404);
}

void RequestHandler::logGetRequest(const std::string &path) const
{
	static int getCounter = 0; // ← vive solo aquí
	getCounter++;

	std::cout << "[GET #" << getCounter << "] " << path << std::endl;
}

// 20260306: Pex
// Auxiliary function created for executing CGIs, works like a pipex
// May need more of this or change this one so it accepts more things than python
std::string executeCGIScript(const std::string &scriptPath, std::string key)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
		return "";

	pid_t pid = fork();
	if (pid == -1)
		return "";

	if (pid == 0)
	{
		// Child process
		dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
		close(pipefd[0]);
		close(pipefd[1]);
		if (key == "python")
			execl("/usr/bin/python3", "python3", scriptPath.c_str(), (char *)NULL);
		else if (key == "php")
			execl("/usr/bin/php", "php", scriptPath.c_str(), (char *)NULL);
		else if (key == "bash")
			execl("/usr/bin/bash", "bash", scriptPath.c_str(), (char *)NULL);
		std::cout << "No exec worked correctly" << std::endl;
		exit(1); // If execl fails
	}
	else
	{
		// Parent process
		close(pipefd[1]);
		char buffer[4096];
		std::string output;
		ssize_t count;
		while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipefd[0]);
		waitpid(pid, NULL, 0);
		return output;
	}
}

// 20260303 - Implemented basic GET request handling, including file reading and response generation.
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet/handlePost/handleDelete
// 20260306 - Added CGI reading
Response RequestHandler::handleGet(const Request &request)
{
	std::string path = request.getPath();

	// API endpoints
	if (path == "/api/name")
		return getName();

	if (path == "/api/background")
		return getBackground();

	if (path == "/api/clothes")
		return getClothes();

	if (path == "/api/hope")
		return getHope();

	if (path == "/trigger500")
		return buildErrorResponse(500);

	if (!isPathSafe(path))
		return buildErrorResponse(403); // Forbidden

	std::string filePath = resolveGetPath(path);
	std::string root = _server.getStaticRoot();
	if (filePath.find(root) != 0)
		return buildErrorResponse(403); // Forbidden

	std::string resolvedPath = filePath;
	logGetRequest(resolvedPath); // Log the resolved path for debugging

	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	// If the file is a CGI script, it is sent to executeCGIScript()
	const LocationConfig *CGIlocation = this->findMatchingLocation(path);
	if (filePath.find("/cgi-bin/") != std::string::npos && CGIlocation != NULL)
	{
		std::vector<std::string> allowedExts = CGIlocation->getCGIExt();
		std::string ext;
		if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".py")
			ext = ".py";
		else if (filePath.size() > 3 && filePath.substr(filePath.size() - 4) == ".php")
			ext = ".php";
		else if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".sh")
			ext = ".sh";

		// If the extension is not valid returns an error
		if (std::find(allowedExts.begin(), allowedExts.end(), ext) == allowedExts.end())
		{
			std::cout << "Extension not allowed" << std::endl;
			return (buildErrorResponse(403));
		}

		std::string cgiOutput;
		if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".py")
			cgiOutput = executeCGIScript(filePath, "python");
		else if (filePath.size() > 3 && filePath.substr(filePath.size() - 4) == ".php")
			cgiOutput = executeCGIScript(filePath, "php");
		else if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".sh")
			cgiOutput = executeCGIScript(filePath, "bash");

		Response response;
		response.setStatusCode(200);
		response.setHeader("Content-Type", "text/html");
		response.setBody(cgiOutput);
		return response;
	}
	std::string content = readFileContent(filePath);
	return buildFileResponse(content, filePath);
}

// 20260223 - Basic method for not allowed response
// main -> handleRequest -> methodNotAllowed
Response RequestHandler::methodNotAllowed()
{
	return buildErrorResponse(405); // Method Not Allowed
									/*
									Response response;
									response.setStatusCode(405);
									response.setHeader("Content-Type", "text/plain; charset=utf-8");
									response.setHeader("Allow", "GET, POST, DELETE");
									response.setBody("405 Method Not Allowed\nThis endpoint does not accept that HTTP method.\n");
									return response;
									*/
}

// 20260223 - switched to route requests based on HTTP method
//  main -> server.run() -> handleClientConnection() -> handleRequest
Response RequestHandler::handleRequest(const Request &request)
{
	if (request.getMethod() == "GET")
		return handleGet(request);
	else if (request.getMethod() == "POST")
		return handlePost(request);
	else if (request.getMethod() == "DELETE")
		return handleDelete(request);
	else
		return methodNotAllowed(); // Protection 405 → Method Not Allowed
}
