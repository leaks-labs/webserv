#include "HttpRequest.hpp"

#include <sstream>

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

size_t HttpRequest::FindRequest(const std::string &buff, size_t pos)
{
    return (buff.find("\r\n\r\n", pos));
}

void HttpRequest::Parse()
{
    std::stringstream stream(get_message());
    if (stream.fail())
        throw std::runtime_error("failed to create stream");
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(stream, token, "\r\n"))
        tokens.push_back(token);
}