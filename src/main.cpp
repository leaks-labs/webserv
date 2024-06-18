#include <csignal>
#include <iostream>
#include <exception>
#include "WebServ.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN);
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
        return 1;
    }

	try
	{
        WebServ* server;
        if (argc == 2) {
            server = new WebServ(argv[1]);
        } else {
            server = new WebServ;
        }
        server->run();
	}
	catch(const std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}