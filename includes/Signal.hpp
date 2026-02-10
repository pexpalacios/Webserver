#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#include "Server.hpp"

//20260210 Terto: Signal handler class to manage graceful shutdown on SIGINT (Ctrl+C)
// main -> SignalHandler::registerSignal() -> SignalHandler::handle() -> sets running to 0 to exit main loop
class SignalHandler 
{
public:
	static volatile		sig_atomic_t running;
	static void			registerSignal();
	static void			handle(int signal);
};

#endif
