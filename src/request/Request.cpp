#include "Request.hpp"

#include <stdexcept>

/* Constructor for Request class */
Request::Request(const std::string& request_raw)
        : request_raw_(request_raw)
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

///* Getter for request_type_ attribute */
//const std::string& Request::GetRequestType() const
//{
//    return request_type_.type; // todo return a struct
//}
//
///* Setter for request_type_ attribute */
//void Request::SetRequestType(const std::string& request_type)
//{
//    request_type_ = request_type;
//}

/* Parse and verify request method */
void Request::Parse()
{

}