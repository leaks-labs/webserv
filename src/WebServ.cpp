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
    server_list_.addServer();
    server_list_.print();
}

WebServ::WebServ(const std::string& config_file)
{
    try
    {
        server_list_.openFile(config_file);
        server_list_.print();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
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
