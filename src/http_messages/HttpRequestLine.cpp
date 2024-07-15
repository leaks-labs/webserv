#include "HttpRequestLine.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "HttpRequest.hpp"
#include "PathFinder.hpp"

// TODO: to be removed
#include <iostream>
// TODO: to be removed

const std::string   HttpRequestLine::kOriginForm = "/";
const std::string   HttpRequestLine::kAbsoluteForm = "http";
const std::string   HttpRequestLine::kAsteriskForm = "*";

const std::map<std::string, bool>   HttpRequestLine::method_map = HttpRequestLine::InitMethodMap();
const std::map<std::string, bool>   HttpRequestLine::target_map = HttpRequestLine::InitTargetMap();

HttpRequestLine::HttpRequestLine()
{
}

HttpRequestLine::HttpRequestLine(const HttpRequestLine &src)
{
    *this = src;
}

HttpRequestLine&    HttpRequestLine::operator=(const HttpRequestLine &rhs)
{
    if (this != &rhs) {
        method_ = rhs.get_method();
        target_ = rhs.get_target();
        http_version_ = rhs.get_http_version();
        line_ = rhs.get_line();
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

const std::string&  HttpRequestLine::get_http_version() const
{
    return http_version_;
}

const std::string&  HttpRequestLine::get_line() const
{
    return line_;
}

void HttpRequestLine::Parse(const std::string &request_line)
{
    line_ = request_line;
    std::vector<std::string> tokens;
    HttpRequest::Split(request_line, " ", tokens);
    if (tokens.size() != 3)
        throw std::runtime_error("400");
    std::string method(tokens[0]);
    std::map<std::string, bool>::const_iterator method_it = method_map.find(method);
    if (method_it == method_map.end())
        throw std::runtime_error("400");
    if (!method_it->second)
        throw std::runtime_error("400");
    method_ = method_it->first;
    std::string s_target(tokens[1]);
    std::map<std::string, bool>::const_iterator target_type_it = InitTargetType(s_target);
    if (target_type_it == target_map.end())
        throw std::runtime_error("400");
    if (!target_type_it->second)
        throw std::runtime_error("400");
    target_.set_type(target_type_it->first);
    std::string::size_type query_pos = s_target.find('?');
    std::string::size_type fragment_pos = s_target.find('#');
    target_.set_target(UrlCleaner(s_target.substr(0, query_pos)));
    target_.set_query(query_pos != std::string::npos ? s_target.substr(query_pos + 1, fragment_pos - query_pos - 1) : "");
    target_.set_fragment(fragment_pos != std::string::npos ? s_target.substr(fragment_pos + 1) : "");
    std::string http_version(tokens[2]);
    if (http_version == "HTTP/0.9"|| http_version == "HTTP/1.0")
        throw std::runtime_error("400");
    else if (http_version != "HTTP/1.1" && http_version != "HTTP/2" && http_version != "HTTP/3")
        throw std::runtime_error("400");
    http_version_ = http_version;
}

std::map<std::string, bool>::const_iterator HttpRequestLine::InitTargetType(const std::string& target)
{
    std::map<std::string, bool>::const_iterator it = target_map.begin();
    for (; it != target_map.end(); ++it)
        if (target.compare(0, it->first.length(), it->first) == 0)
            break;
    return it;
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

std::map<std::string, bool> HttpRequestLine::InitTargetMap()
{
    std::map<std::string, bool> m;
    m[kOriginForm] =    true;
    m[kAbsoluteForm] =  false;
    m[kAsteriskForm] =  false;
    return m;
}

std::string HttpRequestLine::UrlCleaner(const std::string& url) const
{
    std::string res = url;
    size_t pos;
    while ((pos = res.find("%20")) != std::string::npos)
        res.replace(pos, 3, " ");
    return PathFinder::CanonicalizePath(res);
}

void HttpRequestLine::Print() const
{
    std::cout << "\tHttpResquestLine properties:" << std::endl
        << "\t\tmethod: " << get_method() << std::endl
        << "\t\ttarget: " << get_target().get_type() << " " << get_target().get_target() << " " << get_target().get_query() << " " << get_target().get_fragment() << std::endl
        << "\t\tversion: " << get_http_version() << std::endl;
}

HttpRequestLine::Target::Target()
    : type_("/"),
      target_("/")
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

void    HttpRequestLine::Target::set_type(const std::string& type)
{
    type_ = type;
}

void    HttpRequestLine::Target::set_target(const std::string& target)
{
    target_ = target;
}

void    HttpRequestLine::Target::set_query(const std::string& query)
{
    query_ = query;
}

void    HttpRequestLine::Target::set_fragment(const std::string& fragment)
{
    fragment_ = fragment;
}
