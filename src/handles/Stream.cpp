#include "Stream.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <unistd.h>

// TODO: to remove
#include <iostream>
// TODO: to remove

Stream::Stream(int sfd)
    : sfd_(sfd),
      buffer_(kBufSize)
{
}

Stream::Stream(const Stream& src) :
    sfd_(src.get_sfd())
{
}
Stream& Stream::operator=(const Stream& rhs)
{
    if(this != &rhs)
        sfd_ = rhs.get_sfd();
    return *this;
}

Stream::~Stream()
{
    close(sfd_);
}

int Stream::get_sfd() const
{
    return sfd_;
}

void    Stream::Send(std::string& data)
{
    std::cout << "ENTER: Send" << std::endl;
#ifdef __APPLE__
    int send_flags = 0;
#elif __linux__
    int send_flags = MSG_DONTWAIT;
#endif
    ssize_t bytes_sent = send(sfd_, data.c_str(), data.size(), send_flags);
    if (bytes_sent == -1)
        throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
    data.erase(0, bytes_sent);
}

std::string Stream::Read()
{
    std::cout << "ENTER: Read " << std::endl;
    ssize_t bytes_read;
    // TODO: prepare the request or append to complete an incomplete request
    // for now, just consume data
#ifdef __APPLE__
    int recv_flags = 0;
#elif __linux__
    int recv_flags = MSG_DONTWAIT;
#endif
    if ((bytes_read = recv(sfd_, buffer_.data(), buffer_.size(), recv_flags)) == -1)
        throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
    else if (bytes_read == 0)
        throw std::runtime_error("recv() failed: connection closed by peer");
    return std::string(buffer_.data(), bytes_read);
}
