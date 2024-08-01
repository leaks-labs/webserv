#include "Stream.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <unistd.h>

Stream::Stream(int sfd)
    : sfd_(sfd),
      buffer_(kBufSize)
{
}

Stream::~Stream()
{
    Close();
}

int Stream::get_sfd() const
{
    return sfd_;
}

void    Stream::Send(std::string& data)
{
#ifdef __APPLE__
    int send_flags = 0;
#elif __linux__
    int send_flags = MSG_DONTWAIT;
#endif
    ssize_t bytes_sent = send(sfd_, data.c_str(), data.size(), send_flags);
    if (bytes_sent == -1)
        throw std::runtime_error("send() failed:" + std::string(strerror(errno)));
    data.erase(0, bytes_sent);
}

std::string Stream::Read()
{
    ssize_t bytes_read;
#ifdef __APPLE__
    int recv_flags = 0;
#elif __linux__
    int recv_flags = MSG_DONTWAIT;
#endif
    if ((bytes_read = recv(sfd_, buffer_.data(), buffer_.size(), recv_flags)) == -1)
        throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
    else if (bytes_read == 0)
        return std::string();
    return std::string(buffer_.data(), bytes_read);
}

void    Stream::Close()
{
    if (sfd_ != -1) {
        close(sfd_);
        sfd_ = -1;
    }
}
