#include "WebServ.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "EventBroker.hpp"

WebServ::WebServ()
{
    listener_list_.AddDefaultListenerRecords();
    // listener_list_info_.PrintListenerRecords();
    listener_list_.EnableListeners();
}

WebServ::WebServ(const std::string& config_file)
{
    std::ifstream file(config_file.c_str());

    if (file.good() == false)
        throw std::runtime_error("opening config_file failed");

    // TODO: read config_file and initialize a config object
    // TODO: listener_list_info_.AddRecord();
    // TODO: listener_list_info_.CreateListeners(listeners_);
}

WebServ::~WebServ()
{
}

int WebServ::Run() const
{
    EventBroker event_broker(listener_list_);

    return event_broker.Run();
}
