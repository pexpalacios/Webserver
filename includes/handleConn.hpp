#ifndef HANDLECONN_HPP
#define HANDLECONN_HPP
#include "Server.hpp"

//20260207 Terto: Accept a new connection and add it to pollfd array
// main -> server.run() -> Server::run() -> poll() + handleNewConnection() -> handleNewConnection() -> accept() + push_back a fds
void	handleNewConnection(int listenSock, std::vector<struct pollfd>& fds);

//20260207 Terto: Handle an existing client connection (recv, respond, close)
// main -> server.run() -> Server::run() -> poll() + handleClientConnection() -> handleClientConnection() -> recv() + server.readFile() + send() + close()
void	handleClientConnection(int clientSock, Server& server);

#endif
