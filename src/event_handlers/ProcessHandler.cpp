#include "ProcessHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

ProcessHandler::ProcessHandler(StreamHandler& stream_handler, std::string &buffer)
    : stream_handler_(stream_handler),
    buffer_(buffer)
{
}

ProcessHandler::~ProcessHandler()
{
}

void    ProcessHandler::InitHandler()
{
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent | EventTypes::kWriteEvent) == -1)
        throw std::runtime_error("Failed to register ProcessHandler with InitiationDispatcher");
    if (InitiationDispatcher::Instance().DeactivateHandler(stream_handler_) == -1) {
        InitiationDispatcher::Instance().DeactivateHandler(*this);
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
    }
}

EventHandler::Handle    ProcessHandler::get_handle(void) const
{
    return stream_.get_sfd();
}

void    ProcessHandler::HandleTimeout()
{
    // TODO: implement
}

void    ProcessHandler::ReturnToStreamHandler()
{
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
