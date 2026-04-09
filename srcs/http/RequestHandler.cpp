#include "../../includes/http/RequestHandler.hpp"
extern char **environ;

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

// 20260311 - switched to route requests based on HTTP method
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleDelete
Response RequestHandler::handleDelete(const Request &request)
{
	std::string path = request.getPath();

	// Remove query string if present
	size_t q = path.find('?');
	if (q != std::string::npos)
		path = path.substr(0, q);

	if (path == "/api/background")
		return handleDeleteUploadedBackground();

	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveDeletePath(path);
	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	if (!deleteFile(filePath))
		return buildErrorResponse(500);

	std::cout << "[DELETE] resolved path: " << filePath << std::endl;

	return buildNoContentResponse();
}

// 20260307 - Implemented basic POST request handling
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handlePost
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

	if (path == "/api/alive")
		return handleSetAlive();

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
		return ("");

	pid_t pid = fork();
	if (pid == -1)
		return ("");

	if (pid == 0)
	{
		// Child process
		dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
		close(pipefd[0]);
		close(pipefd[1]);
		if (key == "python")
		{
			char *argv[] = {(char *)"python3", (char *)scriptPath.c_str(), NULL};
			execve("/usr/bin/python3", argv, environ);
		}
		else if (key == "php")
		{
			char *argv[] = {(char *)"php", (char *)scriptPath.c_str(), NULL};
			execve("/usr/bin/php", argv, environ);
		}
		else if (key == "bash")
		{
			char *argv[] = {(char *)"bash", (char *)scriptPath.c_str(), NULL};
			execve("/usr/bin/bash", argv, environ);
		}
		return ("");
		exit(1);
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

Response RequestHandler::handleCGI(std::string filePath, const LocationConfig *CGIlocation)
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
		return buildErrorResponse(403);
	}

	std::string CGIOutput;
	if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".py")
		CGIOutput = executeCGIScript(filePath, "python");
	else if (filePath.size() > 3 && filePath.substr(filePath.size() - 4) == ".php")
		CGIOutput = executeCGIScript(filePath, "php");
	else if (filePath.size() > 3 && filePath.substr(filePath.size() - 3) == ".sh")
		CGIOutput = executeCGIScript(filePath, "bash");

	if (CGIOutput == "")
		return buildErrorResponse(500);

	Response response;
	response.setStatusCode(200);
	response.setHeader("Content-Type", "text/html");
	response.setBody(CGIOutput);
	return response;
}

// 20260303 - Implemented basic GET request handling, including file reading and response generation.
// 20260317 - Added autoindex support for GET requests to directories without index.html
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet
// 20260306 - Added CGI reading
Response RequestHandler::handleGet(const Request &request)
{
	std::string path = request.getPath();

	size_t q = path.find('?');
	if (q != std::string::npos)
		path = path.substr(0, q);

	//Redirection endpoints
	if (path == "/redirect")
	{
		const LocationConfig *redirLocation = this->findMatchingLocation(path);
		Response response;
		response.setStatusCode(redirLocation->getRedirectionCode());
		response.setHeader("Location", redirLocation->getRedirection());
		response.setBody("Redirecting...");
		return response;
	}
	if (path == "/external")
	{
		const LocationConfig *redirLocation = this->findMatchingLocation(path);
		Response response;
		response.setStatusCode(redirLocation->getRedirectionCode());
		response.setHeader("Location", redirLocation->getRedirection());
		response.setBody("Redirecting...");
		return response;	
	}

	// API endpoints
	if (path == "/api/name")
		return getName();

	if (path == "/api/background")
		return getBackground();

	if (path == "/api/upload_background")
		return getUploadedBackground();

	if (path == "/api/clothes")
		return getClothes();

	if (path == "/api/hope")
		return getHope();

	if (path == "/api/dialogue")
		return getDialog();

	if (path == "/api/alive")
		return getAlive();

	if (path == "/trigger500")
		return buildErrorResponse(500);

	if (!isPathSafe(path))
		return buildErrorResponse(403);

	std::string filePath = resolveGetPath(path);
	std::string root = _server.getStaticRoot();
	if (filePath.find(root) != 0)
		return buildErrorResponse(403);

	// If the file is a CGI script, it is sent to executeCGIScript()
	const LocationConfig *CGIlocation = this->findMatchingLocation(path);
	if (filePath.find("/cgi-bin/") != std::string::npos && CGIlocation != NULL)
	{
		Response response = handleCGI(filePath, CGIlocation);
		return response;
	}

	std::string resolvedPath = filePath;
	logGetRequest(resolvedPath); // Log the resolved path for debugging

	// 20260317 Autoindex
	if (isDirectory(filePath))
	{
		LocationConfig *location = const_cast<LocationConfig*>(this->findMatchingLocation(path));
		if (hasIndexFile(filePath))
			filePath += "/index.html";
		else if (location && location->getAutoindex())
			return buildAutoindexResponse(filePath, path);
		else
			return buildErrorResponse(403);
	}

	int status = checkFile(filePath);
	if (status != 200)
		return buildErrorResponse(status);

	std::string content = readFileContent(filePath);
	return buildFileResponse(content, filePath);
}

// 20260407 - made this extra function to check if a given method is allowed by the server, previously, this was not checked
int checkMethod(Request request, Server& server)
{
	std::vector<LocationConfig> locs = server.getLocations();

	for (std::vector<LocationConfig>::iterator it = locs.begin(); it != locs.end(); ++it)
	{
		std::vector<std::string> methods = it->getMethods();
		for (std::vector<std::string>::iterator ite = methods.begin(); ite != methods.end(); ++ite)
		{
			if (request.getMethod() == *ite)
				return (true);
		}
	}
	return (false);
}

// 20260223 - switched to route requests based on HTTP method
//  main -> server.run() -> handleClientConnection() -> handleRequest
Response RequestHandler::handleRequest(const Request &request, Server& server)
{
	std::cout << "[METHOD RECEIVED] " << request.getMethod() << std::endl;
	std::cout << "[PATH RECEIVED] " << request.getPath() << std::endl;

	if (!checkMethod(request, server))
		return(methodNotAllowed());

	if (request.getMethod() == "GET")
		return handleGet(request);
	else if (request.getMethod() == "POST")
		return handlePost(request);
	else if (request.getMethod() == "DELETE")
		return handleDelete(request);
	else
		return methodNotAllowed();
}
