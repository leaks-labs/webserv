#ifndef SERVER_LIST_HPP_
# define SERVER_LIST_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Server.hpp"

class ServerList {

    private:
        ServerList(const ServerList& src);
        ServerList&   operator=(const ServerList& rhs);
        std::ifstream file;
        std::vector<Server> servers;
        int loadFile();
    public:
        ServerList();
        ~ServerList();
        void addServer();
        void openFile(const std::string & path);
        void print()const;
        std::vector<Server> const & getServers() const;

};

#endif