#include "WebServ.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "EventBroker.hpp"

const std::string   WebServ::kDefaultConfigFile = "config.txt";

WebServ::WebServ(const std::string& config_file)
{
    server_list_.InitServerList(config_file);
    listener_list_.InitListenerList(server_list_);
    server_list_.Print();
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
