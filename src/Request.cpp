#include "Request.hpp"

/* Constructor for Request class */
Request::Request(const std::string& request_raw)
        : request_raw_(request_raw) {}

/* Copy constructor for Request class */
Request::Request(const Request& request)
        : request_raw_(request.request_raw_), request_type_(request.GetRequestType()) {}

/* Getter for request_raw_ */
const std::string& Request::GetRequestRaw() const {return request_raw_;}

/* Getter and setter for request_type_ attribute */
const std::string& Request::GetRequestType() const {return request_type_;}
void Request::SetRequestType(const std::string& request_type) {request_type_ = request_type;}


