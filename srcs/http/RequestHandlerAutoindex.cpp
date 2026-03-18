#include "../../includes/http/RequestHandler.hpp"

// 20260317 - Auxiliary method to check if a path is a directory, used for autoindexing
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> isDirectory
bool RequestHandler::isDirectory(const std::string& path) const
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
		return S_ISDIR(s.st_mode);
	return false;
}

// 20260317 - Auxiliary method to check if a directory contains an index.html file, used for autoindexing
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> hasIndexFile
bool RequestHandler::hasIndexFile(const std::string& path) const
{
	std::string indexPath = path + "/index.html";
	return access(indexPath.c_str(), F_OK) == 0;
}

// 20260317 - Added autoindex support for GET requests to directories without index.html
//  main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> handleAutoindex -> generateAutoindexHTML
std::string RequestHandler::generateAutoindexHTML(const std::vector<std::string>& files, const std::string& uri)
{
	std::string html = "<html><body>";
	html += "<h1>Index of " + uri + "</h1><ul>";

	size_t i = 0;
	while (i < files.size())
	{
		html += "<li><a href=\"";

		if (uri[uri.size() - 1] == '/')
			html += uri + files[i];
		else
			html += uri + "/" + files[i];

		html += "\">" + files[i] + "</a></li>";

		i++;
	}

	html += "</ul></body></html>";
	return html;
}


// 20260317 - Added autoindex support for GET requests to directories without index.html
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> handleAutoindex -> listDirectory
std::vector<std::string> RequestHandler::listDirectory(const std::string& path)
{
	std::vector<std::string> files;
	DIR* dir = opendir(path.c_str());

	if (!dir)
		return files;

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;

		if (name == "." || name == "..")
			continue;

		files.push_back(name);
	}

	closedir(dir);
	return files;
}


//20260317 - Added autoindex support for GET requests to directories without index.html
// main -> server.run() -> handleClientConnection() -> handleRequest -> handleGet -> buildAutoindexResponse
Response RequestHandler::buildAutoindexResponse(const std::string& path, const std::string& uri)
{
	std::vector<std::string> files = listDirectory(path);
	std::string html = generateAutoindexHTML(files, uri);

	Response res;
	res.setStatusCode(200);
	res.setHeader("Content-Type", "text/html");
	res.setBody(html);

	return res;
}
