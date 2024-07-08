#include "Cgi.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <unistd.h>

// TODO: to remove
#include <iostream>
#include <Server.hpp>
// TODO: to remove

Cgi::Cgi(){};

Cgi::Cgi(int sfd[2]) :
    buffer_(kBufSize),
    reading_(false)
{
    std::memcpy(sfds_, sfd, sizeof(int) * 2);
}

Cgi::Cgi(Cgi const & src)
{
    *this = src;
}

Cgi &Cgi::operator=(Cgi const & src)
{
    if(this != &src)
    {
        std::memcpy(sfds_, src.get_sfds(), sizeof(int) * 2);
    }
    return *this;
}

Cgi::~Cgi()
{
    close(sfds_[0]);
    close(sfds_[1]);
}

int *Cgi::get_sfds()const
{
    return const_cast<int *>(sfds_);
}

int Cgi::get_sfd() const
{
    if(reading_)
        return sfds_[0];
    return sfds_[1];
}

int    Cgi::Send(std::string& data)
{
    std::cout << "ENTER: Send" << std::endl;
#ifdef __APPLE__
    int send_flags = 0;
#elif __linux__
    int send_flags = MSG_DONTWAIT;
#endif
    ssize_t bytes_sent = send(sfds_[0], data.c_str(), data.size(), send_flags);
    if (bytes_sent == -1)
        throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
    data.erase(0, bytes_sent);
    reading_ = data.empty();
    return reading_;
}

std::string Cgi::Read()
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
    if ((bytes_read = recv(sfds_[1], buffer_.data(), buffer_.size(), recv_flags)) == -1)
        throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
    else if (bytes_read == 0)
        throw std::runtime_error("recv() failed: connection closed by peer");
    return std::string(buffer_.data(), bytes_read);
}
