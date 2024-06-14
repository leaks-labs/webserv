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

/* Getter for request_raw_ attribute */
const std::string& Request::GetRequestRaw() const
{
    return request_raw_;
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

/* Getter for is_request_complete_ attribute */
bool Request::GetIsRequestComplete() const
{
    return is_request_complete_;
}

/* Setter for is_complete_ attribute (only accessible by definition) */
void Request::SetIsComplete(bool is_request_complete)
{
    is_request_complete_ = is_request_complete;
}

/* Parse and verify request method */
void Request::Parse()
{
    std::istringstream request(GetRequestRaw());
    std::string buf;

    try
    {
        std::getline(request, buf);
        if (!request.good()) {
            throw std::runtime_error(INPUT_ERROR);
        }
        GetRequestLinePrivate().Parse(buf);
        for (size_t i = 0; std::getline(request, buf); ++i) {
            GetRequestHeaderPrivate().Parse(buf);
        }
    }
    catch (std::exception& e)
    {
        throw e;
    }
}