#include "WebServ.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

const std::string   WebServ::kDefaultConfigFile = "config.txt";

WebServ::WebServ(const std::string& config_file)
{
    server_list_.InitServerList(config_file);
    listener_list_.InitListenerList(server_list_);
    //server_list_.Print();
}

WebServ::~WebServ()
{
}

void    WebServ::Run() const
{
   if (server_list_.Size() < 1)
        throw std::runtime_error("no servers to run");
    EventBroker event_broker(listener_list_);
    event_broker.Run();
}

Server WebServ::FindServer(const int client_sfd, const std::string& name) const
{
    typedef std::vector<Server>::const_iterator Iterator;
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