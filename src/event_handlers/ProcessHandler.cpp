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

ProcessHandler::ProcessHandler(StreamHandler& stream_handler, HttpResponse & response)
    : stream_handler_(stream_handler),
    response_(response)
{
    if (InitiationDispatcher::Instance().DeactivateHandler(stream_handler_) == -1)
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
}

ProcessHandler::~ProcessHandler()
{
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
    std::cout << "ProcessHandler line 40 : return to stream handler" << std::endl;
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
