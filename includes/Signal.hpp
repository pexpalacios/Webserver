#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#include "Server.hpp"

class SignalHandler 
{
public:
	static volatile		sig_atomic_t running;
	static void			registerSignal();
	static void			handle(int signal);
};

#endif
