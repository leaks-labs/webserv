#include "HttpRequest.hpp"

#include <sstream>
#include <iostream>

int HttpRequest::Split(const std::string& str, const std::string& delim, std::vector<std::string>& tokens)
{
    size_t start = 0, end;
    std::string token;
    do {
        end = str.find(delim, start);
        std::string token = str.substr(start, end - start);
        if (token.empty())
            return -1;
        else
            tokens.push_back(token);
        start = end + delim.length();
    } while (end != std::string::npos);
    return 0;
}

HttpRequest::HttpRequest(int acceptor_fd)
    : acceptor_fd_(acceptor_fd),
      is_complete_(false),
      status_code_(200),
      server_(&ServerList::Instance().FindServer(acceptor_fd, "")),
      location_(&server_->FindLocation("/"))
{
}

HttpRequest::HttpRequest(int acceptor_fd, int status_code)
    : acceptor_fd_(acceptor_fd),
      is_complete_(true),
      status_code_(status_code),
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

HttpRequestLine&    HttpRequest::get_request_line()
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

std::string&    HttpRequest::get_body()
{
    return body_.get_body();
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

void    HttpRequest::set_host(const std::string& host)
{
    header_.AddOneHeader("HOST", host);
}

void    HttpRequest::AppendToRequest(std::string& message)
{
    try
    {
        if (!request_line_.IsComplete())
            request_line_.Parse(message);
        if (!message.empty() && !header_.IsComplete()) {
            header_.Parse(message, HttpHeader::kParseRequest);
            if (header_.IsComplete()) {
                server_ = &ServerList::Instance().FindServer(acceptor_fd_, get_host());
                location_ = &server_->FindLocation(request_line_.get_target().get_target());
                if (!header_.NeedBody())
                    body_.set_is_complete(true);
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
        iss >> std::noskipws >> code;
        if (iss.fail() || !iss.eof() || code < 100 || code > 599)
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
