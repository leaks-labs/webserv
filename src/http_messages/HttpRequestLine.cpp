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

void HttpRequestLine::set_target(const RequestTarget &target)
{
    request_target_ = request_target;
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
    std::map<std::string, bool>::const_iterator it = method_map.find(method);
    if (it == method_map.end())
        throw std::runtime_error("Bad Request");
    if (!it->second)
        throw std::runtime_error("Unauthorized");
    set_method(*it);
    std::string target(tokens.at(1));
    Target target_type = FindRequestTargetType(target);

        throw std::runtime_error("bad request"); //todo improve
    RequestTarget request_target = {target, target_type};
    set_request_target(request_target);
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