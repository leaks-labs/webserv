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

StreamHandler::StreamHandler(int sfd)
    : 
    stream_(sfd),
    request_count(0)
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
            if (EventTypes::IsWriteEvent(event_type) && request_count > 0) {
                // TODO: send the first compliete response. For now, just send a string.
                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                stream_.Send(response);
                // TODO: if there is bytes not sent, we need to keep the response in the response queue with the remaining data.
                // or if all bytes sent, we need to remove the response from the response queue.
                // For now, just remove the response from the map.
                --request_count;

                // modify the filter to remove write filter
                // only if there is no more response to send for this fd
                // TODO: check if the response queue is empty (COMPLETE or NOT COMPLETE) for this fd. For now, just check the size_t
                if (request_count == 0 && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
                    throw std::runtime_error("Failed to delete write filter for a socket");
            } else if (EventTypes::IsReadEvent(event_type)) {
                // TODO: add the string return by Read to the request queue
                // For now, just add the request in the map.
                ++request_count;
                stream_.Read();

                // modify the filter to add EVFILT_WRITE
                // only if the request queue is empty for this fd
                // TODO: check if the request queue is equel to 1 (of COMPLETE and VALID requests) for this fd. For now, just check the size_t
                if(1 == 0) // if request is ready and for cgi
                    AddCgiHandler();
                else
                {
                    if (request_count == 1 && InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
                        throw std::runtime_error("failed to add write filter for a socket:" + std::string(strerror(errno)));
                }
            }
        }
        catch(const std::exception& e)
        {
            InitiationDispatcher::Instance().RemoveHandler(this);
            throw;
        }
    }
}

void StreamHandler::AddCgiHandler()
{
    try
    {
        cgi_handlers_.push_back(CgiHandler(const_cast<StreamHandler&>(*this), "coucou"));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    StreamHandler::HandleTimeout()
{
    // TODO: implement
}
