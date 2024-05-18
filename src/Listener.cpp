#include "Listener.hpp"

# include <stdexcept>

# include <sys/socket.h>
# include <unistd.h>

Listener::Listener(const struct addrinfo& address)
{
    if ((sfd_ = socket(address.ai_family, address.ai_socktype, address.ai_protocol)) == -1)
        throw std::runtime_error("socket creation failed");
    try
    {
        // TODO: set SO_KEEPALIVE ?
        int yes = true;
        if (setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1
            || setsockopt(sfd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
            // TODO: perror("setsockopt");
            throw std::runtime_error("setsockopt failed");
        }
        if (bind(sfd_, address.ai_addr, address.ai_addrlen) == -1) {
            // TODO: perror("bind");
            throw std::runtime_error("socket binding failed");
        }
            // TODO: unlink after binding?
        if (listen(sfd_, SOMAXCONN) == -1)
            // TODO: perror("listen");
            throw std::runtime_error("socket listening failed");
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