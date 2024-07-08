#include "CgiHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove


CgiHandler::CgiHandler(EventHandler& stream_handler, std::string const & request)
    : stream_handler_(stream_handler)
{
    std::cout << request << std::endl;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sfd_) == -1)
        throw std::runtime_error("Failed create socketpair: " + std::string(strerror(errno)));
    cgi_ = Cgi(sfd_);
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent | EventTypes::kWriteEvent) == -1)
        throw std::runtime_error("Failed to register CgiHandler with InitiationDispatcher");
    if (InitiationDispatcher::Instance().DeactivateHandler(stream_handler) == -1) {
        InitiationDispatcher::Instance().DeactivateHandler(*this);
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
    }
}

CgiHandler::~CgiHandler()
{
}
 
EventHandler::Handle    CgiHandler::get_handle(void) const
{
    return cgi_.get_sfd();
}

void    CgiHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER CgiHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing Cgi" << std::endl;
        ReturnToStreamHandler();
        return;
    } else if (EventTypes::IsWriteEvent(event_type)) {
        std::string request = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
        
        if (cgi_.Send(request) && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1) {
            ReturnToStreamHandler();
            throw std::runtime_error("Failed to delete write filter for a socket");
        }
    } else if (EventTypes::IsReadEvent(event_type)) {
        cgi_.Read();
        ReturnToStreamHandler();
        return;
    }
}

void    CgiHandler::HandleTimeout()
{
    // TODO: implement
}

void    CgiHandler::ReturnToStreamHandler()
{
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
