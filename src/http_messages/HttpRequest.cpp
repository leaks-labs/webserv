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
    try {
        std::vector<std::string> tokens;
        HttpMessage::Split(get_message(), "\r\n", tokens);
        if (tokens.empty())
            throw std::runtime_error("Bad Request");
        get_protected_request_line().Parse(tokens.at(0));
        for (size_t i = 1; i < tokens.size(); i++) {
            get_protected_header().Parse(tokens.at(i));
        }
        get_protected_body().SearchBody(*this);
    } catch (const std::exception &e) {
        std::string key = e.what();
        const std::string& value = HttpMessage::status_map.at(key);
        set_status(std::make_pair(key, value));
    }
}