#include "../includes/Signal.hpp"

volatile sig_atomic_t SignalHandler::running = 1;

void SignalHandler::registerSignal() 
{
	std::signal(SIGINT, SignalHandler::handle);
}

void SignalHandler::handle(int signal) 
{
	if (signal == SIGINT) {
		std::cout << "\nSIGINT received. Shutting down..." << std::endl;
		SignalHandler::running = 0;
	}
}
