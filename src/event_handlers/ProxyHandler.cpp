#include "ProxyHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

struct addrinfo*    ProxyHandler::ConvertToAddrInfo(const std::string& url)
{
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int                 status;
    struct addrinfo*    res;
    if ((status = getaddrinfo(url.c_str(), "http", &hints, &res)) != 0)
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));

    return res;
}

ProxyHandler::ProxyHandler(StreamHandler& stream_handler, const struct addrinfo& address, const std::string& url, HttpResponse& response)
    : stream_handler_(stream_handler),
      response_(response),
#ifdef __APPLE__
      stream_(socket(address.ai_family, address.ai_socktype, address.ai_protocol))
#elif __linux__
      stream_(socket(address.ai_family, address.ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, address.ai_protocol))
#endif
{
    if (stream_.get_sfd() == -1)
        throw std::runtime_error("socket() failed to create a socket");
#ifdef __APPLE__
    if (fcntl(stream_.get_sfd(), F_SETFD, FD_CLOEXEC) == -1 || fcntl(stream_.get_sfd(), F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl() failed");
#endif
    if (connect(stream_.get_sfd(), address.ai_addr, address.ai_addrlen) == -1 && errno != EINPROGRESS)
        throw std::runtime_error("connect() failed to connect to the remote host: " + std::string(strerror(errno)));
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent | EventTypes::kWriteEvent) == -1)
        throw std::runtime_error("Failed to register ProxyHandler with InitiationDispatcher");

    if (stream_handler.UnRegister() == -1) {
        InitiationDispatcher::Instance().RemoveHandler(this);
        throw std::runtime_error("Failed to unregister StreamHandler");
    }

    response.set_request_host(url);
    request_ = response.get_complete_request();

    // TODO: or update the response with a 500 error or something instead of throwing?
}

ProxyHandler::~ProxyHandler()
{
}

EventHandler::Handle    ProxyHandler::get_handle(void) const
{
    return stream_.get_sfd();
}

void    ProxyHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER ProxyHandler: event " << event_type << std::endl;
    if (EventTypes::IsWriteEvent(event_type)) {
        // TODO: send the request. For now, just send a string.
        std::string request = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
        stream_.Send(request);

        // TODO: modify the filter to remove write filter
        // only if there is no more response bytes to send for this fd.
        // For now, just remove the write filter.
        if (InitiationDispatcher::Instance().DelWriteFilter(*this) == -1) {
            // TODO: update the response with a 500 error or something
            ReturnToStreamHandler();
            throw std::runtime_error("Failed to delete write filter for a socket");
        }
    } else if (EventTypes::IsReadEvent(event_type)) {
        // TODO: add the string return by Read to the response; for now, just consume data
        stream_.Read();

        // TODO: update the response with a 500 error or something if the response is not valid.

        // TODO: delete this event handler and reactivate the stream handler
        // only if the response in complete from the backend server.
        // If the response is not complete, we need to keep the handler alive.
        // At some point, close if we see that the response is not valid, and update the response with a 500 error or something.
        // For now, just delete this event handler because the work in intended done.
        ReturnToStreamHandler();
        return; // Do NOT remove this return. It is important to be sure to return here.
    } else if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing proxy" << std::endl;
        // TODO: either close this handler because all is alright or update the response with a 500 error or something,
        // because the response is not complete and that an error occured
        ReturnToStreamHandler();
        return; // Do NOT remove this return. It is important to be sure to return here.
    }
}

void    ProxyHandler::HandleTimeout()
{
    // TODO: implement
}

void    ProxyHandler::ReturnToStreamHandler()
{
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
