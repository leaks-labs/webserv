#include "HttpStatusLine.hpp"

#include <sstream>

#include "HttpRequest.hpp"

HttpStatusLine::HttpStatusLine()
    : is_complete_(false),
      http_version_("HTTP/1.1"),
      status_code_(200),
      reason_phrase_("OK")
{
}

HttpStatusLine::HttpStatusLine(int code)
    : is_complete_(true),
      http_version_("HTTP/1.1"),
      status_code_(code)
{
    SetCodeAndPhrase(code);
}

HttpStatusLine::HttpStatusLine(const HttpStatusLine& src)
    : is_complete_(src.is_complete_),
      http_version_(src.http_version_),
      status_code_(src.status_code_),
      reason_phrase_(src.reason_phrase_),
      buffer_(src.buffer_)
{
}

HttpStatusLine& HttpStatusLine::operator=(const HttpStatusLine& rhs)
{
    if (this != &rhs) {
        is_complete_ = rhs.is_complete_;
        http_version_ = rhs.http_version_;
        status_code_ = rhs.status_code_;
        reason_phrase_ = rhs.reason_phrase_;
        buffer_ = rhs.buffer_;
    }
    return *this;
}

HttpStatusLine::~HttpStatusLine()
{
}

const std::string&  HttpStatusLine::get_http_version() const
{
    return http_version_;
}

std::string HttpStatusLine::get_status_code_str() const
{
    std::ostringstream oss;
    oss << status_code_;
    return oss.str();
}

int HttpStatusLine::get_status_code() const
{
    return status_code_;
}

const std::string&  HttpStatusLine::get_reason_phrase() const
{
    return reason_phrase_;
}

// void    HttpStatusLine::Parse(std::string& message)
// {
//     (void)message;
//     // TODO: Implement
// }

void    HttpStatusLine::SetCodeAndPhrase(int code)
{
    const std::map<int, std::string>&   status_map = HttpRequest::status_map;
    std::map<int, std::string>::const_iterator it = status_map.find(code);
    status_code_ = code;
    if (it != status_map.end())
        reason_phrase_ = it->second;
    else
        reason_phrase_ = "Unknown Status Code";
}

bool    HttpStatusLine::IsComplete() const
{
    return is_complete_;
}

std::string HttpStatusLine::GetFormatedStatusLine() const
{
    return get_http_version() + " " + get_status_code_str() + " " + get_reason_phrase() + "\r\n";
}

void    HttpStatusLine::Clear()
{
    is_complete_ = false;
    http_version_ = "HTTP/1.1";
    status_code_ = 200;
    reason_phrase_ = "OK";
    buffer_.clear();
}

void    HttpStatusLine::Print() const
{
    std::cout << "HTTP Status Line: " << GetFormatedStatusLine();
}

size_t  HttpStatusLine::FindEndOfStatusLine(const std::string& buff)
{
    size_t  pos = buff.find("\r\n");
    return pos != std::string::npos ? pos + kTerminatorSize : kNotFoundEnd;
}
