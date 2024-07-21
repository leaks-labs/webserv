#include "HttpRequestLine.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "HttpCodeException.hpp"
#include "HttpRequest.hpp"
#include "PathFinder.hpp"

// TODO: to be removed
#include <iostream>
// TODO: to be removed

const std::string   HttpRequestLine::Target::kOriginForm = "/";
const std::string   HttpRequestLine::Target::kAbsoluteForm = "http";
const std::string   HttpRequestLine::Target::kAsteriskForm = "*";

const std::map<std::string, bool>   HttpRequestLine::method_map = HttpRequestLine::InitMethodMap();
const std::map<std::string, bool>   HttpRequestLine::Target::target_map = HttpRequestLine::Target::InitTargetMap();

HttpRequestLine::HttpRequestLine()
    : is_complete_(false)
{
}

HttpRequestLine::HttpRequestLine(const HttpRequestLine &src)
{
    *this = src;
}

HttpRequestLine&    HttpRequestLine::operator=(const HttpRequestLine &rhs)
{
    if (this != &rhs) {
        is_complete_ = rhs.is_complete_;
        method_ = rhs.method_;
        target_ = rhs.target_;
        http_version_ = rhs.http_version_;
        buffer_ = rhs.buffer_;
    }
    return *this;
}

HttpRequestLine::~HttpRequestLine()
{
}

const std::string&  HttpRequestLine::get_method() const
{
    return method_;
}

const HttpRequestLine::Target&   HttpRequestLine::get_target() const
{
    return target_;
}

HttpRequestLine::Target&    HttpRequestLine::get_target()
{
    return target_;
}

const std::string&  HttpRequestLine::get_http_version() const
{
    return http_version_;
}

void HttpRequestLine::Parse(std::string& message)
{
    size_t initial_buffer_size = buffer_.length();
    buffer_ += message;
    if (buffer_.size() > kMaxRequestLineSize)
        throw HttpCodeExceptions::UrlTooLongException();
    size_t pos = buffer_.find("\r\n");
    if (pos == std::string::npos) {
        message.clear();
        return;
    }

    buffer_.erase(pos);
    size_t  bytes_to_trim_in_message = buffer_.length() - initial_buffer_size + 2;
    message.erase(0, bytes_to_trim_in_message);
    is_complete_ = true;
    std::vector<std::string> tokens;
    if (HttpRequest::Split(buffer_, " ", tokens) == -1)
        throw HttpCodeExceptions::BadRequestException();
    buffer_.clear();
    if (tokens.size() != 3)
        throw HttpCodeExceptions::BadRequestException();
    buffer_.clear();
    std::map<std::string, bool>::const_iterator method_it = method_map.find(tokens[0]);
    if (method_it == method_map.end() || !method_it->second)
        throw HttpCodeExceptions::BadRequestException();
    method_ = tokens[0];
    target_.InitTargetType(tokens[1]);
    target_.set_complete_url(tokens[1]);
    if (tokens[2] != "HTTP/0.9"&& tokens[2] != "HTTP/1.0"
        && tokens[2] != "HTTP/1.1" && tokens[2] != "HTTP/2" && tokens[2] != "HTTP/3")
        throw HttpCodeExceptions::BadRequestException();
    else if (tokens[2] != "HTTP/1.1")
        throw HttpCodeExceptions::HttpVersionNotSupportedException();
    http_version_ = tokens[2];
}

bool    HttpRequestLine::IsComplete() const
{
    return is_complete_;
}

std::string HttpRequestLine::GetFormatedRequestLine() const
{
    return method_ + " " + target_.get_complete_url() + " " + http_version_ + "\r\n";
}

void HttpRequestLine::Print() const
{
    std::cout << "\tHttpResquestLine properties:" << std::endl
        << "\t\tmethod: " << get_method() << std::endl
        << "\t\ttarget: " << get_target().get_type() << " " << get_target().get_target() << " " << get_target().get_query() << " " << get_target().get_fragment() << std::endl
        << "\t\tversion: " << get_http_version() << std::endl;
}

std::map<std::string, bool> HttpRequestLine::InitMethodMap() {
    std::map<std::string, bool> m;
    m["OPTIONS"] =  false;
    m["GET"] =      true;
    m["HEAD"] =     false;
    m["POST"] =     true;
    m["PUT"] =      false;
    m["DELETE"] =   true;
    m["TRACE"] =    false;
    m["CONNECT"] =  false;
    return m;
}

std::string HttpRequestLine::Target::UrlCleaner(const std::string& url)
{
    std::string res = url;
    size_t pos;
    while ((pos = res.find("%20")) != std::string::npos)
        res.replace(pos, 3, " ");
    return PathFinder::CanonicalizePath(res);
}

HttpRequestLine::Target::Target()
    : type_("/"),
      target_("/"),
      complete_url("/")
{
}

HttpRequestLine::Target::Target(const Target& src)
{
    *this = src;
}

HttpRequestLine::Target&    HttpRequestLine::Target::operator=(const Target& rhs)
{
    if (this != &rhs) {
        type_ = rhs.get_type();
        target_ = rhs.get_target();
        query_ = rhs.get_query();
        fragment_ = rhs.get_fragment();
        complete_url = rhs.complete_url;
    }
    return *this;
}

HttpRequestLine::Target::~Target()
{
}

const std::string&  HttpRequestLine::Target::get_type() const
{
    return type_;
}

const std::string&  HttpRequestLine::Target::get_target() const
{
    return target_;
}

const std::string&  HttpRequestLine::Target::get_query() const
{
    return query_;
}

const std::string&  HttpRequestLine::Target::get_fragment() const
{
    return fragment_;
}

const std::string&  HttpRequestLine::Target::get_complete_url() const
{
    return complete_url;
}

void    HttpRequestLine::Target::set_type(const std::string& type)
{
    type_ = type;
}

void    HttpRequestLine::Target::set_target(const std::string& target)
{
    target_ = target;
    UpdateCompleteUrl();
}

void    HttpRequestLine::Target::set_query(const std::string& query)
{
    query_ = query;
    UpdateCompleteUrl();
}

void    HttpRequestLine::Target::set_fragment(const std::string& fragment)
{
    fragment_ = fragment;
    UpdateCompleteUrl();
}

void    HttpRequestLine::Target::set_complete_url(const std::string& url)
{
    complete_url = url;

    std::string::size_type query_pos = url.find('?');
    std::string::size_type fragment_pos = url.find('#');
    target_ = UrlCleaner(url.substr(0, query_pos));
    query_ = (query_pos != std::string::npos ? url.substr(query_pos + 1, fragment_pos - query_pos - 1) : "");
    fragment_ = (fragment_pos != std::string::npos ? url.substr(fragment_pos + 1) : "");
}

void    HttpRequestLine::Target::InitTargetType(const std::string& target)
{
    std::map<std::string, bool>::const_iterator it = target_map.begin();
    for (; it != target_map.end(); ++it)
        if (target.compare(0, it->first.length(), it->first) == 0)
            break;
    if (it == target_map.end() || !it->second)
        throw HttpCodeExceptions::BadRequestException();
    type_ = it->first;
}

bool    HttpRequestLine::Target::IsTargetEmpty() const
{
    return target_.empty();
}

void    HttpRequestLine::Target::ClearTarget()
{
    target_.clear();
}

std::map<std::string, bool> HttpRequestLine::Target::InitTargetMap()
{
    std::map<std::string, bool> m;
    m[kOriginForm] =    true;
    m[kAbsoluteForm] =  false;
    m[kAsteriskForm] =  false;
    return m;
}

void    HttpRequestLine::Target::UpdateCompleteUrl()
{
    complete_url = target_;
    if (!query_.empty())
        complete_url += "?" + query_;
    if (!fragment_.empty())
        complete_url + "#" + fragment_;
}
