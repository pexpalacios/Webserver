#include "../includes/Server.hpp"
#include "../includes/http/RequestHandler.hpp"


//20260325 - Implemented request receiving and parsing logic, with error handling for invalid requests and internal server errors.
// main -> server.run() -> handleClientConnection() -> checkMaxSize()
bool Server::checkMaxSize(long long contentLength) const
{
	if (contentLength < 0)
		return false;

	if (contentLength > _maxBodySize)
		return false;

	return true;
}


//20260311 - Implemented request receiving and parsing logic, with error handling for invalid requests and internal server errors.
// main -> server.run() -> handleClientConnection() -> recvRequest
std::string Server::recvRequest(int clientSock)
{
	char buffer[4096];
	std::string raw;

	size_t contentLength = 0;
	bool headersParsed = false;
	size_t headerEnd = std::string::npos;

	while (true)
	{
		ssize_t bytesRead = recv(clientSock, buffer, sizeof(buffer), 0);

		if (bytesRead > 0)
		{
			raw.append(buffer, bytesRead);

			if (!headersParsed)
			{
				headerEnd = raw.find("\r\n\r\n");

				if (headerEnd != std::string::npos)
				{
					headersParsed = true;

					size_t pos = raw.find("Content-Length:");
					if (pos != std::string::npos)
					{
						size_t start = pos + 15;
						while (raw[start] == ' ')
							start++;

						size_t end = raw.find("\r\n", start);
						std::string lenStr = raw.substr(start, end - start);

						long long contentLengthLL = 0;
						std::istringstream iss(lenStr);

						if (!(iss >> contentLengthLL) || contentLengthLL < 0)
						{
							std::cerr << "[ERROR] Invalid Content-Length" << std::endl;
							return "";
						}

						std::cout << "[CONTENT-LENGTH] " << contentLengthLL << std::endl;

						// 20260325 Terto: Check if content length exceeds maximum allowed size
						if (!checkMaxSize(contentLengthLL))
						{
							std::cout << "[ERROR] Payload Too Large (early)" << std::endl;

							Response res;
							res.setStatusCode(413);
							res.setHeader("Content-Type", "text/html");
							res.setBody("Payload too large\n");

							std::string responseStr = res.toString();
							send(clientSock, responseStr.c_str(), responseStr.size(), 0);

							std::cout << "[RESPONSE STATUS] 413" << std::endl;
							std::cout << "-----------------------------------------------------\n" << std::endl;

							close(clientSock);
							return "";
						}

						contentLength = static_cast<size_t>(contentLengthLL);
					}

					if (contentLength == 0)
						break;
				}
			}

			if (headersParsed)
			{
				size_t bodyStart = headerEnd + 4;

				if (raw.size() >= bodyStart + contentLength)
					break;
			}
		}
		else if (bytesRead == 0)
		{
			break;
		}
		else
		{
			std::cerr << "recv() failed" << std::endl;
			return "";
		}
	}

	return raw;
}

// Cambiado por el uso de PollServer, ahora el manejo de conexiones se hace ahí, no en Server
/*
//20260311 - Implemented request receiving and parsing logic, with error handling for invalid requests and internal server errors.
// main -> server.run() -> handleClientConnection()
void Server::handleClientConnection(int clientSock, Server& server)
{
	std::string raw = recvRequest(clientSock);

	if (raw.empty())
	{
		close(clientSock);
		return;
	}

	std::cout << "Request received:\n" << raw << std::endl;

	Request request;
	if (!request.parse(raw))
	{
		std::cerr << "[ERROR] Invalid HTTP request" << std::endl;

		RequestHandler handler(server);
		Response responseObj = handler.handleBadRequest();

		std::string responseStr = responseObj.toString();
		send(clientSock, responseStr.c_str(), responseStr.size(), 0);

		std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
		std::cout << "-----------------------------------------------------\n" << std::endl;

		close(clientSock);
		return;
	}

	RequestHandler handler(server);
	Response responseObj;

	try
	{
		responseObj = handler.handleRequest(request);
	}
	catch (...)
	{
		std::cerr << "[ERROR] Internal Server Error" << std::endl;
		responseObj = handler.handleInternalServerError();
	}

	std::string response = responseObj.toString();
	send(clientSock, response.c_str(), response.size(), 0);

	std::cout << "[RESPONSE STATUS] " << responseObj.getStatusCode() << std::endl;
	std::cout << "-----------------------------------------------------\n" << std::endl;

	close(clientSock);
}
*/
