#include "HttpRequestLine.hpp"

#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <string>

const RequestMethod method_dictionary[N_METHOD] = {
        {"OPTIONS", 7, false},
        {"GET", 3, true},
        {"HEAD", 4, false},
        {"POST", 4,true},
        {"PUT", 3, false},
        {"DELETE", 6, true},
        {"TRACE", 5, false},
        {"CONNECT", 7, false},
};

HttpRequestLine::HttpRequestLine()
{
}


HttpRequestLine::~HttpRequestLine()
{
}

const RequestMethod& HttpRequestLine::get_request_method() const
{
    return request_method_;
}

void HttpRequestLine::set_request_method(const RequestMethod &request_method)
{
    request_method_ = request_method;
}

const RequestTarget& HttpRequestLine::get_request_target() const
{
    return request_target_;
}

void HttpRequestLine::set_request_target(const RequestTarget &request_target)
{
    request_target_ = request_target;
}

const std::string& HttpRequestLine::get_request_http() const
{
    return request_http_;
}

void HttpRequestLine::set_request_http(const std::string & request_http)
{
    request_http_ = request_http;
}

void HttpRequestLine::Parse(const std::string &request_line)
{
    std::stringstream stream(request_line);
    if (stream.fail())
        throw std::runtime_error("failed to create stream");
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(stream, token, ' '))
        tokens.push_back(token);
    if (tokens.size() != 3)
        throw std::runtime_error("bad request"); //todo improve
    std::string method(tokens.at(1));
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);
    size_t dictionary = FindMethod(method_dictionary, method);
    if (dictionary == N_METHOD || !method_dictionary[dictionary].is_used)
        throw std::runtime_error("bad request"); //todo improve
    set_request_method(method_dictionary[dictionary]);
    std::string target(tokens.at(2));
    RequestTargetType target_type = FindRequestTargetType(target);
    if (target_type == UNKNOWN_FORM)
        throw std::runtime_error("bad request"); //todo improve
    RequestTarget request_target = {target, target_type};
    set_request_target(request_target);
}

size_t HttpRequestLine::FindMethod(
        const RequestMethod* dictionary,
        const std::string &request_method)
{
    size_t i;
    for (i = 0; i < N_METHOD; ++i) {
        if (dictionary[i].method == request_method)
            break;
    }
    return i;
}