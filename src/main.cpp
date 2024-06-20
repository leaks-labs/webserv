#include <csignal>
#include <iostream>
#include <exception>

#include "WebServ.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        std::cerr << "ERROR: signal() failed" << std::endl;
        return 1;
    }

    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
        return 1;
    }

    WebServ* server = NULL;
	try
	{
        if (argc == 2)
            server = new WebServ(argv[1]);
        else
            server = new WebServ;
        int ret = server->Run();
        delete server;
        return ret < 0 ? 1 : 0;
	}
	catch(const std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
        delete server;
		return 1;
	}
}
