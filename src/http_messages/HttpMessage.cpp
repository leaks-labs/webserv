#include "HttpMessage.hpp"

const std::map<std::string, std::string>
        HttpMessage::status_map = HttpMessage::init_status_map();

HttpMessage::HttpMessage()
        : is_complete_(false)
{
}

HttpMessage::~HttpMessage()
{
}

void HttpMessage::Split(
        const std::string& str,
        const std::string& delim,
        std::vector<std::string>& tokens)
{
    size_t i = 0, j = 0;
    while ((j = str.find(delim, i)) != std::string::npos) {
        std::string token(str.substr(i, j - i));
        if (!token.empty())
            tokens.push_back(token);
        if ((i = str.find_first_not_of(delim, j)) == std::string::npos)
            return;
    }
    tokens.push_back(str.substr(i));
}

const HttpRequestLine& HttpMessage::get_request_line() const
{
    return request_line_;
}

HttpRequestLine& HttpMessage::get_protected_request_line()
{
    return request_line_;
}

const HttpHeader& HttpMessage::get_header() const
{
    return header_;
}

HttpHeader& HttpMessage::get_protected_header()
{
    return header_;
}

const HttpBody& HttpMessage::get_body() const
{
    return body_;
}

HttpBody& HttpMessage::get_protected_body()
{
    return body_;
}

const std::pair<std::string, std::string>& HttpMessage::get_status() const
{
    return status_;
}

const std::string& HttpMessage::get_message() const
{
    return message_;
}

bool HttpMessage::get_is_complete() const
{
    return is_complete_;
}

void HttpMessage::set_status(const std::pair<std::string, std::string>& status)
{
    status_ = status;
}

void HttpMessage::set_message(const std::string& message)
{
    message_ = message;
}

void HttpMessage::set_is_complete(bool is_complete)
{
    is_complete_ = is_complete;
}

std::map<std::string, std::string> HttpMessage::init_status_map()
{
    std::map<std::string, std::string> m;
    m["OK"] =                               "200";
    m["CREATED"] =                          "201";
    m["NO CONTENT"] =                       "204";
    m["Moved Permanently"] =                "301";
    m["Found"] =                            "302";
    m["Not Modified"] =                     "304";
    m["Bad Request"] =                      "400";
    m["Unauthorized"] =                     "401";
    m["Forbidden"] =                        "403";
    m["Not Found"] =                        "404";
    m["Method Not Allowed"] =               "405";
    m["Proxy Authentication Required"] =    "407";
    m["Request Timeout"] =                  "408";
    m["Payload Too Large"] =                "413";
    m["Request Header Fields Too Large"] =  "431";
    return m;
}