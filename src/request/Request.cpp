#include "Request.hpp"

#include <sstream>
#include <stdexcept>

/* Constructor for Request class */
Request::Request()
        : is_request_complete_(false)
{
}

/* Destructor for Request class */
Request::~Request()
{
}

/* Copy constructor for Request class */
Request::Request(const Request &request)
        : request_line_(request.GetRequestLine()),
          request_header_(request.GetRequestHeader()),
          request_(request.GetRequest()),
          is_request_complete_(request.GetIsRequestComplete())
{
}

Request &Request::operator=(const Request &request)
{
    request_line_ = request.GetRequestLine();
    request_header_ = request.GetRequestHeader();
    request_ = request.GetRequest();
    is_request_complete_ = GetIsRequestComplete();
    return *this;
}

/* Getter for request_line_ attribute */
const RequestLine& Request::GetRequestLine() const
{
    return request_line_;
}

/* Private getter for request_line_ attribute (non const for full access) */
RequestLine &Request::GetRequestLinePrivate()
{
    return request_line_;
}

/* Getter for request_header_ attribute */
const RequestHeader& Request::GetRequestHeader() const
{
    return request_header_;
}

/* Private getter for request_header_ attribute (non const for full access) */
RequestHeader &Request::GetRequestHeaderPrivate()
{
    return request_header_;
}

/* Getter for request_ attribute */
const std::string& Request::GetRequest() const
{
    return request_;
}

/* Setter for request_ attribute (only accessible by definition) */
void Request::SetRequest(const std::string &request)
{
    request_ = request;
}

/* Getter for is_request_complete_ attribute */
bool Request::GetIsRequestComplete() const
{
    return is_request_complete_;
}

/* Setter for is_complete_ attribute (only accessible by definition) */
void Request::SetIsRequestComplete(bool is_request_complete)
{
    is_request_complete_ = is_request_complete;
}

void Request::AddRequestData(const std::string& request_data)
{
    if (request_data.empty() || GetIsRequestComplete())
        throw std::runtime_error("Empty string or already complete request");
    SetRequest(GetRequest() + request_data);
    if (IsRequestComplete(GetRequest()))
        SetIsRequestComplete(true);
}

/* Parse and verify request method */
void Request::Parse()
{
    std::istringstream request(GetRequest());
    std::string buf;
    try
    {
        std::getline(request, buf);
        if (!request.good()) {
            throw std::runtime_error(INPUT_ERROR);
        }
        GetRequestLinePrivate().Parse(buf);
        for (size_t i = 0; std::getline(request, buf); ++i) {
            ;
        }
    }
    catch (std::exception& e)
    {
        throw e;
    }
}