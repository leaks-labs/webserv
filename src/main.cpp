#include <cerrno>
#include <csignal>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>

#include "Server.hpp"
#include "WebServ.hpp"

#include "PathFinder.hpp"

int main(int argc, char** argv)
{
	try
	{
        if (argc > 2)
            throw std::runtime_error("only one config file can be specified");
        if (signal(SIGINT, SIG_IGN) == SIG_ERR)
            throw std::runtime_error("signal() failed: " + std::string(strerror(errno)));
        WebServ server(argc == 2 ? argv[1] : WebServ::kDefaultConfigFile);
        server.Run();
        return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
