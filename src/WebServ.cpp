#include "WebServ.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "InitiationDispatcher.hpp"
# include "ServerList.hpp"

const std::string   WebServ::kDefaultConfigFile = "config.txt";

WebServ::WebServ(const std::string& config_file)
{
    ServerList::Instance().InitServerList(config_file);
    acceptor_records_.InitAcceptors(ServerList::Instance());
    ServerList::Instance().Print();
}

WebServ::~WebServ()
{
}

void    WebServ::Run() const
{
   if (ServerList::Instance().Size() < 1)
        throw std::runtime_error("no servers to run");
    InitiationDispatcher::Instance().HandleEvents();
    InitiationDispatcher::Instance().Clear();
}

Server WebServ::FindServer(const int client_sfd, const std::string& name) const
{
    typedef std::vector<const Server&>::const_iterator Iterator;
    std::vector<Server> matched;
    for (size_t i = 0; i < server_list_.Size(); i++)
        if(ListenerList::IsSameAddr(client_sfd, server_list_.GetServers()[i].get_addr()))
            matched.push_back(server_list_.GetServers()[i]);
    for (Iterator it = matched.begin(); it != matched.end(); it++)
    {
        if(it->HasServerName(name))
            return *it;
    }
    return matched[0];
}