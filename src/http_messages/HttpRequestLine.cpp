#include "HttpRequestLine.hpp"
#include "HttpMessage.hpp"

#include <algorithm>
#include <vector>
#include <stdexcept>

const std::map<std::string, bool> HttpRequestLine::method_map =
        HttpRequestLine::InitMethodMap();

const std::map<std::string, bool> HttpRequestLine::target_map =
        HttpRequestLine::InitTargetMap();

HttpRequestLine::HttpRequestLine()
{
}


HttpRequestLine::~HttpRequestLine()
{
}

const std::pair<std::string, bool>& HttpRequestLine::get_method() const
{
    return method_;
}

const Target& HttpRequestLine::get_target() const
{
    return target_;
}

const std::string& HttpRequestLine::get_http_version() const
{
    return http_version_;
}

void HttpRequestLine::set_method(const std::pair<std::string, bool>& method)
{
    method_ = method;
}

void HttpRequestLine::set_target(const Target &target)
{
    target_ = target;
}

void HttpRequestLine::set_http_version(const std::string &http_version)
{
    http_version_ = http_version;
}

void HttpRequestLine::Parse(const std::string &request_line)
{
    std::vector<std::string> tokens;
    HttpMessage::Split(request_line, " ", tokens);
    if (tokens.size() != 3)
        throw std::runtime_error("Bad Request");
    std::string method(tokens.at(0));
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);
    std::map<std::string, bool>::const_iterator method_it = method_map.find(method);
    if (method_it == method_map.end())
        throw std::runtime_error("Bad Request");
    if (!method_it->second)
        throw std::runtime_error("Unauthorized");
    set_method(*method_it);
    std::string s_target(tokens.at(1));
    std::map<std::string, bool>::const_iterator target_type_it = InitTargetType(s_target);
    if (target_type_it == target_map.end())
        throw std::runtime_error("Bad Request");
    if (!target_type_it->second)
        throw std::runtime_error("Unauthorized");
    Target target;
    target.type = *target_type_it;
    target.target = s_target;
    target.is_cgi = s_target.substr(0, 9) == "/cgi-bin/";
    set_target(target);
    std::string http_version(tokens.at(2));
    if (http_version != "HTTP/0.9"|| http_version != "HTTP/1.0")
        throw std::runtime_error("Unauthorized");
    if (http_version != "HTTP/1.1"|| http_version != "HTTP/2" || http_version != "HTTP/3")
        throw std::runtime_error("Bad Request");
    set_http_version(http_version);
}

std::map<std::string, bool>::const_iterator HttpRequestLine::InitTargetType(
        const std::string &target)
{
    std::map<std::string, bool>::const_iterator it = target_map.begin();
    while (it != target_map.end()) {
        if (it->first == target.substr(0, it->first.length()))
            break;
    }
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
    m[ORIGIN_FORM] =    true;
    m[ABSOLUTE_FORM] =  true;
    m[AUTHORITY_FORM] = true;
    m[ASTERISK_FORM] =  false;
    return m;
}