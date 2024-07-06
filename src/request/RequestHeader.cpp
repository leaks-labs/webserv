#include "RequestHeader.hpp"

/* Constructor for RequestHeader class */
RequestHeader::RequestHeader()
{
}

/* Destructor for RequestHeader class */
RequestHeader::~RequestHeader()
{
}

/* Copy constructor for RequestHeader class */
RequestHeader::RequestHeader(const RequestHeader &request_header)
    : header_map_(request_header.header_map_)
{
}

/* Assignment constructor for RequestHeader class */
RequestHeader &RequestHeader::operator=(const RequestHeader &request_header)
{
    header_map_ = request_header.header_map_;
    return *this;
}