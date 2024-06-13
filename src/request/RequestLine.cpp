#include "RequestLine.hpp"

/* Constructor for RequestLine class */
RequestLine::RequestLine()
{
}

/* Destructor for RequestLine class */
RequestLine::~RequestLine()
{
}

/* Copy constructor for RequestLine class */
RequestLine::RequestLine(const RequestLine &request_line)
        : request_type_(request_line.GetRequestType()),
          request_url_(request_line.GetRequestUrl()),
          request_http_version_(request_line.GetRequestHttpVersion())
{
}

/* Assignment constructor for RequestLine class */
RequestLine &RequestLine::operator=(const RequestLine &request_line)
{
    request_type_ = request_line.GetRequestType();
    request_url_ = request_line.GetRequestUrl();
    request_http_version_ = request_line.GetRequestHttpVersion();
    return *this;
}

/* Getter for request_type_ attribute */
const std::string& RequestLine::GetRequestType() const
{
    return request_type_;
}

/* Setter for request_type_ attribute */
void RequestLine::SetRequestType(const std::string &request_type)
{
    request_type_ = request_type;
}

/* Getter for request_url_ attribute */
const s_request_url& RequestLine::GetRequestUrl() const
{
    return request_url_;
}

/* Setter for request_url_ attribute */
void RequestLine::SetRequestUrl(const s_request_url &request_url)
{
    request_url_ = request_url;
}

/* Getter for request_http_version_ attribute */
const std::string& RequestLine::GetRequestHttpVersion() const
{
    return request_http_version_;
}

/* Setter for request_http_version_ attribute */
void RequestLine::SetRequestHttpVersion(const std::string &request_http_version)
{
    request_http_version_ = request_http_version;
}
