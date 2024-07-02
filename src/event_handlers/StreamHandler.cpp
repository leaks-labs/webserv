#include "StreamHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

StreamHandler::StreamHandler(int acceptor_sfd, int sfd)
    : stream_(acceptor_sfd, sfd)
{
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
        throw std::runtime_error("Failed to register StreamHandler with InitiationDispatcher");
}

StreamHandler::~StreamHandler()
{
}

EventHandler::Handle    StreamHandler::get_handle() const
{
    return stream_.get_sfd();
}

void    StreamHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER StreamHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing stream" << std::endl;
        InitiationDispatcher::Instance().RemoveHandler(this);
        return; // Do NOT remove this return. It is important to be sure to return here.
    } else {
        try
        {
            if (EventTypes::IsWriteEvent(event_type) && stream_.get_request_count() > 0) {
                stream_.Send();

                // modify the filter to remove write filter
                // only if there is no more response to send for this fd
                // TODO: check if the response queue is empty (COMPLETE or NOT COMPLETE) for this fd. For now, just check the size_t
                if (stream_.get_request_count() == 0 && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
                    throw std::runtime_error("Failed to delete write filter for a socket");
            } else if (EventTypes::IsReadEvent(event_type)) {
                stream_.Read();

                // modify the filter to add EVFILT_WRITE
                // only if the request queue is empty for this fd
                // TODO: check if the request queue is equel to 1 (of COMPLETE and VALID requests) for this fd. For now, just check the size_t
                if (stream_.get_request_count() == 1 && InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
                    throw std::runtime_error("failed to add write filter for a socket:" + std::string(strerror(errno)));
            }
        }
        catch(const std::exception& e)
        {
            InitiationDispatcher::Instance().RemoveHandler(this);
            throw;
        }
    }
}
