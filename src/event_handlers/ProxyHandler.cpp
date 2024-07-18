#include "ProxyHandler.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "HttpBody.hpp"
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
    : error_occured_while_handle_event_(false),
      stream_handler_(stream_handler),
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
    response_.set_request_host(url);
    request_ = response_.GetCompleteRequet();
    response_.Clear();
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
        if (EventTypes::IsReadEvent(event_type) || EventTypes::IsWriteEvent(event_type)) {
            try
            {
                if (EventTypes::IsWriteEvent(event_type)) {
                    stream_.Send(request_);
                    if (request_.empty() && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
                        throw std::runtime_error("Failed to delete write filter for a socket");
                } else if (EventTypes::IsReadEvent(event_type)) {
                    buffer_ = stream_.Read();
                    response_.AppendToResponse(buffer_);
                }
            }
            catch(const std::exception& e)
            {
                response_.SetResponseToErrorPage(502);
            }
            if (response_.IsComplete()) {
                ReturnToStreamHandler();
                return; // Do NOT remove this return. It is important to be sure to return here.
            }
        } else if (EventTypes::IsCloseEvent(event_type)) {
                std::cout << "closing proxy" << std::endl;
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
    int err = stream_handler_.ReRegister();
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err == -1)
        throw std::runtime_error("Failed to reactivate Stream Handler");
}
