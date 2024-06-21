#include "WebServ.hpp"

#include <fstream>
#include <iostream>

#include "EventBroker.hpp"

const std::string   WebServ::kDefaultConfigFile = "config.txt";

WebServ::WebServ(const std::string& config_file)
{
    server_list_.OpenFile(config_file);
    for (ServerList::Iterator it = server_list_.begin(); it != server_list_.end(); ++it)
        it->set_addr(listener_list_.AddListenerRecord(it->get_host().c_str(), it->get_port()));
    server_list_.Print();
    // listener_list_info_.PrintListenerRecords();
    listener_list_.EnableListeners();
}

WebServ::~WebServ()
{
}

int WebServ::Run() const
{
    EventBroker event_broker(listener_list_);

    return event_broker.Run();
}
