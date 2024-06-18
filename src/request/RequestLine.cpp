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
        : request_uri_(request_line.GetRequestUri()),
          request_type_(request_line.GetRequestType()),
          request_http_version_(request_line.GetRequestHttpVersion())
{
}

/* Assignment constructor for RequestLine class */
RequestLine &RequestLine::operator=(const RequestLine &request_line)
{
    request_uri_ = request_line.GetRequestUri();
    request_type_ = request_line.GetRequestType();
    request_http_version_ = request_line.GetRequestHttpVersion();
    return *this;
}

/* Getter for request_uri_ attribute */
const s_request_uri& RequestLine::GetRequestUri() const
{
    return request_uri_;
}

/* Setter for request_uri_ attribute (only accessible by definition) */
void RequestLine::SetRequestUri(const s_request_uri &request_uri)
{
    request_uri_ = request_uri;
}

/* Getter for request_type_ attribute */
const std::string& RequestLine::GetRequestType() const
{
    return request_type_;
}

/* Setter for request_type_ attribute (only accessible by definition) */
void RequestLine::SetRequestType(const std::string &request_type)
{
    request_type_ = request_type;
}

/* Getter for request_http_version_ attribute */
const std::string& RequestLine::GetRequestHttpVersion() const
{
    return request_http_version_;
}

/* Setter for request_http_version_ attribute (only accessible by definition) */
void RequestLine::SetRequestHttpVersion(const std::string &request_http_version)
{
    request_http_version_ = request_http_version;
}

void RequestLine::Parse(const std::string& request_line)
{
}
