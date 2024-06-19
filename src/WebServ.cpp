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
    servers.push_back(new Server());
}

WebServ::WebServ(const std::string& config_file)
{
    int err;

    std::ifstream file(config_file.c_str());
    if (file.good() == false)
        throw std::runtime_error("opening config_file failed");
    ConfigLoader loader(file);
    err = loader.loadFileConfig(servers);
    if(err)
    {
        std::cout << "Config file error at line: " << err << std::endl;
        throw std::runtime_error("Config file Error");
    }
    loader.print(servers);
    // TODO: read config_file and initialize a config object
    // TODO: listener_list_info_.AddRecord();
    // TODO: listener_list_info_.CreateListeners(listeners_);
}

WebServ::~WebServ()
{
}

int WebServ::run() const
{
    EventBroker event_broker(listener_list_);

    return event_broker.run();
}
