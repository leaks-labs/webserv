#include "Stream.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include <sys/socket.h>
#include <unistd.h>

// TODO: to remove
#include <iostream>
#include <Response.hpp>
#include <Server.hpp>
// TODO: to remove

Stream::Stream(int acceptor_sfd, int sfd)
    : acceptor_sfd_(acceptor_sfd),
      sfd_(sfd),
      request_count(0),
      buffer_(kBufSize)
{
}

Stream::~Stream()
{
    close(sfd_);
}

int Stream::get_sfd() const
{
    return sfd_;
}

std::size_t Stream::get_request_count() const
{
    return request_count;
}

void    Stream::Send()
{
    std::cout << "ENTER: Send" << std::endl;
    // TODO: send the response
    // TODO: for now, just echo back;

    const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
    ssize_t bytes_sent;
#ifdef __APPLE__
    int send_flags = 0;
#elif __linux__
    int send_flags = MSG_DONTWAIT;
#endif
    if ((bytes_sent = send(sfd_, response.c_str(), response.size(), send_flags)) == -1)
        throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
    // TODO: if bytes_sent < response.size(), we need to keep the response in the response queue with the remaining data.
    // or if bytes_sent == response.size(), we need to remove the response from the response queue in the future ClientList.
    // For now, just remove the response from the map.
    --request_count;
}

void    Stream::Read()
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
    std::string str(buffer_.data(), bytes_read);
    //std::cout << buffer_.data() << std::endl;
    Response response(acceptor_sfd_, "abc.com", "/buro/");
    std::cout << response.GetText() << std::endl;
    // TODO: add string to the request queue

    // TODO: the future ClientList will add the count of VALID requests only if the request is complete
    // For now, just add the request in the map.
    //request_count;
    ++request_count;
}
