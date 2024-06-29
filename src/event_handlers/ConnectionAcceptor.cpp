#include "ConnectionAcceptor.hpp"

#include <stdexcept>

#include <unistd.h>

#include "InitiationDispatcher.hpp"
#include "StreamHandler.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

ConnectionAcceptor::ConnectionAcceptor(const struct addrinfo& address)
    : acceptor_(address)
{
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
        throw std::runtime_error("Failed to register ConnectionAcceptor with InitiationDispatcher");
}

ConnectionAcceptor::~ConnectionAcceptor()
{
}

EventHandler::Handle    ConnectionAcceptor::get_handle() const
{
    return acceptor_.get_sfd();
}

void    ConnectionAcceptor::HandleEvent(EventTypes::Type event_type)
{
    if (!(event_type & EventTypes::kReadEvent))
        return;
    std::cout << "ENTER ConnectionAcceptor: event " << event_type << std::endl;
    int new_sfd = -1;
    try
    {
        new_sfd = acceptor_.Accept();
        new StreamHandler(new_sfd);
    }
    catch(const std::exception& e)
    {
        if (new_sfd != -1)
            close(new_sfd);
        throw;
    }
}
