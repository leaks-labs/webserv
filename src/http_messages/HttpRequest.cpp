#include "HttpRequest.hpp"

#include <sstream>
#include <iostream>

const std::map<int, std::string>    HttpRequest::status_map = HttpRequest::InitStatusMap();

void    HttpRequest::Split(const std::string& str, const std::string& delim, std::vector<std::string>& tokens)
{
    size_t start = 0, end;
    std::string token;
    do {
        end = str.find(delim, start);
        std::string token = str.substr(start, end - start);
        if (token.empty())
            throw std::runtime_error("400");
            // TODO: or something else
        else
            tokens.push_back(token);
        start = end + delim.length();
    } while (end != std::string::npos && start < str.length());
}

HttpRequest::HttpRequest(int acceptor_fd)
    : acceptor_fd_(acceptor_fd),
      is_complete_(false),
      status_code_(200),
      server_(&ServerList::Instance().FindServer(acceptor_fd, "")),
      location_(&server_->FindLocation("/"))
{
}

HttpRequest::HttpRequest(const HttpRequest& src)
{
    *this = src;
}

HttpRequest&    HttpRequest::operator=(const HttpRequest& rhs)
{
    if (this != &rhs) {
        acceptor_fd_ = rhs.acceptor_fd_;
        is_complete_ = rhs.is_complete_;
        status_code_ = rhs.status_code_;
        server_ = rhs.server_;
        location_ = rhs.location_;
        request_line_ = rhs.request_line_;
        header_ = rhs.header_;
        body_ = rhs.body_;
    }
    return *this;
}

HttpRequest::~HttpRequest()
{
}

int HttpRequest::get_status_code() const
{
    return status_code_;
}

const Server&   HttpRequest::get_server() const
{
    return *server_;
}

const Location& HttpRequest::get_location() const
{
    return *location_;
}

const HttpRequestLine&  HttpRequest::get_request_line() const
{
    return request_line_;
}

const HttpHeader&   HttpRequest::get_header() const
{
    return header_;
}

const HttpBody& HttpRequest::get_body() const
{
    return body_;
}

std::string HttpRequest::get_host() const
{
    try
    {
        return header_.get_header_map().at("HOST");
    }
    catch(const std::exception& e)
    {
        return "";
    }
}

void    HttpRequest::AppendToRequest(std::string& message)
{
    try
    {
        if (!request_line_.IsComplete())
            request_line_.Parse(message);
        if (!message.empty() && !header_.IsComplete()) {
            header_.Parse(message);
            if (header_.IsComplete()) {
                if (!header_.NeedBody())
                    is_complete_ = true;
                else if (header_.IsContentLength())
                    body_.SetMode(HttpBody::kModeContentLength, location_->get_bodymax(), header_.GetContentLength());
                else
                    body_.SetMode(HttpBody::kModeTransferEncodingChunked,location_->get_bodymax());
            }
        }
        if (!message.empty() && !body_.IsComplete())
            body_.Parse(message);
        if (body_.IsComplete())
            is_complete_ = true;
    }
    catch (const std::exception &e) {
        std::istringstream iss(e.what());
        int code;
        if (!(iss >> code))
            code = 500;
        status_code_ = code;
        is_complete_ = true;
    }
}

bool    HttpRequest::IsComplete() const
{
    return is_complete_;
}

std::string HttpRequest::GetCompleteRequest() const
{
    return request_line_.GetFormatedRequestLine() + header_.GetFormatedHeader() + body_.get_body();
}

bool    HttpRequest::KeepAlive() const
{
    try
    {
        return (header_.get_header_map().at("CONNECTION") != "close");
    }
    catch(const std::exception& e)
    {
        return true;
    }
}

std::map<int, std::string>  HttpRequest::InitStatusMap()
{
    std::map<int, std::string>  m;
    m[200] = "OK";
    // m[201] = "CREATED";
    m[204] = "No Content";
    // m[301] = "Moved Permanently";
    // m[302] = "Found";
    // m[304] = "Not Modified";
    m[400] = "Bad Request";
    // m[401] = "Unauthorized";
    // m[403] = "Forbidden";
    m[404] = "Not Found";
    m[405] = "Method Not Allowed";
    // m[407] = "Proxy Authentication Required";
    m[408] = "Request Timeout";
    m[413] = "Content Too Large";
    m[414] = "URI Too Long";
    m[431] = "Request Header Fields Too Large";
    m[500] = "Internal Server Error";
    return m;
}
