#include "HttpStatusLine.hpp"

#include <iostream>
#include <sstream>

#include "HttpCodeException.hpp"
#include "HttpRequest.hpp"


const std::vector<int>              HttpStatusLine::codes_requiring_close_ = HttpStatusLine::InitCodesRequiringClose();
const std::map<int, std::string>    HttpStatusLine::status_code_map_ = HttpStatusLine::InitStatusCodeMap();

const std::map<int, std::string>&   HttpStatusLine::get_status_code_map()
{
    return status_code_map_;
}

HttpStatusLine::HttpStatusLine()
    : is_complete_(false),
      http_version_("HTTP/1.1"),
      status_code_(200),
      reason_phrase_("OK")
{
}

HttpStatusLine::HttpStatusLine(int code)
    : is_complete_(false),
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

const std::vector<int>& HttpStatusLine::get_codes_requiring_close() const
{
    return codes_requiring_close_;
}

void    HttpStatusLine::Parse(std::string& message)
{
    size_t initial_buffer_size = buffer_.length();
    buffer_ += message;
    size_t pos = buffer_.find("\r\n");
    if (pos == std::string::npos) {
        message.clear();
        return;
    }
    buffer_.erase(pos);
    size_t  bytes_to_trim_in_message = buffer_.length() - initial_buffer_size + 2;
    message.erase(0, bytes_to_trim_in_message);
    is_complete_ = true;
    for (std::string::iterator it = buffer_.begin(); it != buffer_.end(); ++it) {
        if (*it == '\r')
            *it = ' ';
    }
    std::vector<std::string> tokens;
    if (HttpRequest::Split(buffer_, " ", tokens) == -1)
        throw HttpCodeExceptions::BadGatewayException();
    buffer_.clear();
    if (tokens.size() < 2)
        throw HttpCodeExceptions::BadGatewayException();
    if (tokens[0] != "HTTP/0.9"&& tokens[0] != "HTTP/1.0"
        && tokens[0] != "HTTP/1.1" && tokens[0] != "HTTP/2" && tokens[0] != "HTTP/3")
        throw HttpCodeExceptions::BadGatewayException();
    http_version_ = tokens[0];
    std::istringstream iss(tokens[1]);
    iss >> std::noskipws >> status_code_;
    if (iss.fail() || !iss.eof() || status_code_ < 100 || status_code_ > 599)
        throw HttpCodeExceptions::BadGatewayException();
    if (tokens.size() > 2) {
        std::string reason_phrase;
        for (size_t i = 2; i < tokens.size(); ++i) {
            reason_phrase += tokens[i];
            if (i != tokens.size() - 1)
                reason_phrase += " ";
        }
        reason_phrase_ = reason_phrase;
    } else {
        SetCodeAndPhrase(status_code_);
    }
}

void    HttpStatusLine::SetCodeAndPhrase(int code)
{
    std::map<int, std::string>::const_iterator it = status_code_map_.find(code);
    status_code_ = code;
    if (it != status_code_map_.end())
        reason_phrase_ = it->second;
    else
        reason_phrase_.clear();
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

const std::vector<int>  HttpStatusLine::InitCodesRequiringClose()
{
    std::vector<int>    res;
    res.push_back(400);
    res.push_back(408);
    res.push_back(413);
    res.push_back(414);
    res.push_back(431);
    res.push_back(500);
    res.push_back(501);
    res.push_back(502);
    res.push_back(505);
    return res;
}

const std::map<int, std::string>  HttpStatusLine::InitStatusCodeMap()
{
    std::map<int, std::string>  m;
    m[200] = "OK";
    m[201] = "Created";
    m[204] = "No Content";
    m[301] = "Moved Permanentely";
    m[308] = "Permanent Redirect";
    m[400] = "Bad Request";
    m[403] = "Forbidden";
    m[404] = "Not Found";
    m[405] = "Method Not Allowed";
    m[408] = "Request Timeout";
    m[413] = "Content Too Large";
    m[414] = "URI Too Long";
    m[431] = "Request Header Fields Too Large";
    m[500] = "Internal Server Error";
    m[501] = "Not Implemented";
    m[502] = "Bad Gateway";
    m[504] = "Gateway Timeout";
    m[505] = "HTTP Version Not Supported";
    return m;
}
