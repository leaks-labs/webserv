#include "Listener.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

Listener::Listener(const struct addrinfo& address)
#ifdef __APPLE__
    : sfd_(socket(address.ai_family, address.ai_socktype, address.ai_protocol))
#elif __linux__
    : sfd_(socket(address.ai_family, address.ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, address.ai_protocol))
#endif
{
    if (sfd_ == -1)
        throw std::runtime_error("socket() failed to create a socket");
#ifdef __APPLE__
    if (fcntl(sfd_, F_SETFD, FD_CLOEXEC) == -1 || fcntl(sfd_, F_SETFL, O_NONBLOCK) == -1) {
        close(sfd_);
        throw std::runtime_error("fcntl() failed");
    }
#endif
    try
    {
        // TODO: set SO_KEEPALIVE ?
        int yes = 1;
        if (setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1
            || setsockopt(sfd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
            throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
        }
        if (bind(sfd_, address.ai_addr, address.ai_addrlen) == -1)
            throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
        if (listen(sfd_, SOMAXCONN) == -1)
            throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
    }
    catch(const std::exception& e)
    {
        close(sfd_);
        throw;
    }
    
}

Listener::~Listener()
{
    close(sfd_);
}

int Listener::get_sfd() const
{
    return sfd_;
}
