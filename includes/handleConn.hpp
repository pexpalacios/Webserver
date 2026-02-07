#ifndef HANDLECONN_HPP
#define HANDLECONN_HPP
#include "Server.hpp"

// 20260207 Terto: Accept a new connection and add it to pollfd array
void	handleNewConnection(int listenSock, std::vector<struct pollfd>& fds);

// 20260207 Terto: Handle an existing client connection (recv, respond, close)
void	handleClientConnection(int clientSock, const std::string& staticRoot);

#endif
