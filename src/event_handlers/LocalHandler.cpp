#include "LocalHandler.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove


LocalHandler::LocalHandler(StreamHandler& stream_handler, std::string const & request)
    : stream_handler_(stream_handler),
    stream_(OpenFile()),
    request_(request)
{
    if(stream_.get_sfd() == -1)
        throw std::runtime_error("Failed to open file");
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
        throw std::runtime_error("Failed to register ProxyHandler with InitiationDispatcher");
    if (InitiationDispatcher::Instance().DeactivateHandler(stream_handler) == -1) {
        InitiationDispatcher::Instance().DeactivateHandler(*this);
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
    }
}

LocalHandler::~LocalHandler()
{
}
 
int LocalHandler::OpenFile()
{
    int err;

    err = open("data/file.html", O_RDONLY);
    if(err == -1)
        err = open("data/error404.html", O_RDONLY);
    return(err);
}

EventHandler::Handle    LocalHandler::get_handle(void) const
{
    return stream_.get_sfd();
}

void    LocalHandler::HandleEvent(EventTypes::Type event_type)
{
    //std::cout << "ENTER LocalHandler: event " << event_type << std::endl;
    if(!EventTypes::IsReadEvent(event_type))
        return;
    std::string r = stream_.Read();
    if(r.empty())
    {
        ReturnToStreamHandler();
        return;
    }
    response_ += r;
}

void    LocalHandler::HandleTimeout()
{
    // TODO: implement
}

void    LocalHandler::ReturnToStreamHandler()
{
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
