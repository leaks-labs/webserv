#include "WebServ.hpp"

#include <fstream>
#include <stdexcept>

#include "InitiationDispatcher.hpp"
# include "ServerList.hpp"

const std::string   WebServ::kDefaultConfigFile = "config.txt";

WebServ::WebServ(const std::string& config_file)
{
    ServerList::Instance().InitServerList(config_file);
    acceptor_records_.InitAcceptors(ServerList::Instance());
}

WebServ::~WebServ()
{
}

void    WebServ::Run() const
{
   if (ServerList::Instance().Size() < 1)
        throw std::runtime_error("no servers to run");
    InitiationDispatcher::Instance().HandleEvents(InitiationDispatcher::kTimeWaitingEvent);
    InitiationDispatcher::Instance().Clear();
}