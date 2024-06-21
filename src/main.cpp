#include <csignal>
#include <exception>
#include <iostream>
#include <string>

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

	try
	{
        WebServ server(argc == 2 ? argv[1] : WebServ::kDefaultConfigFile);
        int ret = server.Run();
        return ret < 0 ;
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}
}
