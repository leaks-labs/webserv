#include "Listener.hpp"

#include <stdexcept>

#include <sys/socket.h>
#include <unistd.h>

Listener::Listener(const struct addrinfo& address) : sfd_(socket(address.ai_family, address.ai_socktype, address.ai_protocol))
{
    if (sfd_ == -1)
        throw std::runtime_error("socket() failed to create a socket");
    try
    {
        // TODO: set SO_KEEPALIVE ?
        int yes = 1;
        if (setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1
            || setsockopt(sfd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
            throw std::runtime_error("setsockopt() failed");
            // TODO: perror("setsockopt");
        }
        if (bind(sfd_, address.ai_addr, address.ai_addrlen) == -1)
            throw std::runtime_error("bind() failed");
            // TODO: perror("bind");
        if (listen(sfd_, SOMAXCONN) == -1)
            throw std::runtime_error("listen() failed");
            // TODO: perror("listen");
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
