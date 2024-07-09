#include "StreamHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include <unistd.h>

#include "InitiationDispatcher.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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
                std::string r = stream_.Read();
                Decode(r);

                // modify the filter to add EVFILT_WRITE
                // only if the request queue is empty for this fd
                // TODO: check if the request queue is equel to 1 (of COMPLETE and VALID requests) for this fd. For now, just check the size_t
                if (request_count == 1 && InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
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

void    StreamHandler::HandleTimeout()
{
    // TODO: implement
}

void   StreamHandler::Decode(std::string& buffer)
{
    size_t i = HttpRequest::FindRequest(buffer, 0);
    HttpRequest request;
    request.set_message(buffer.substr(0, i));
    request.Parse();
    response_queue_.push_back(HttpResponse(request));
}

/*
void    StreamHandler::Execute(const HttpMessage& request)
{
    ;
}

void    StreamHandler::Encode(const HttpMessage& response)
{
    ;
}
*/