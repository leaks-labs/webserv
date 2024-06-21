#include "WebServ.hpp"

#include <fstream>
#include <iostream>

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

int WebServ::Run() const
{
    EventBroker event_broker(listener_list_);

    return event_broker.Run();
}
