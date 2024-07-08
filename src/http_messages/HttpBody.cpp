#include "HttpBody.hpp"

HttpBody::HttpBody()
        : is_complete_(false)
{
}

HttpBody::~HttpBody()
{
}

const std::pair<std::string, std::string>& HttpBody::get_transfer_type() const
{
    return transfer_type_;
}

void HttpBody::set_transfer_type(const std::pair<std::string, std::string>& transfer_type)
{
    transfer_type_ = transfer_type;
}

const std::string& HttpBody::get_body() const
{
    return body_;
}

void HttpBody::set_body(const std::string &body, size_t pos)
{
    body_ += body;
    if (pos == body_.size())
        is_complete_ = true;
}

bool HttpBody::get_is_complete() const
{
    return is_complete_;
}

void HttpBody::set_is_complete(bool is_complete)
{
    is_complete_ = is_complete;
}
