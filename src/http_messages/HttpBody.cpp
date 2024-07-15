#include "HttpBody.hpp"

#include <cstdlib>
#include <stdexcept>

HttpBody::HttpBody()
    : required_length_(0),
      is_transfer_encoding_chunked_(false),
      is_complete_(false)
{
}

HttpBody::HttpBody(const HttpBody& src)
{
    *this = src;
}

HttpBody&   HttpBody::operator=(const HttpBody& rhs)
{
    if (this != &rhs)
        body_ = rhs.get_body();
    return *this;
}

HttpBody::~HttpBody()
{
}

const std::string&  HttpBody::get_body() const
{
    return body_;
}

bool    HttpBody::IsComplete() const
{
    return is_complete_;
}

size_t  HttpBody::get_size() const
{
    return body_.length();
}

void    HttpBody::set_required_length(size_t length)
{
    required_length_ = length;
}

void    HttpBody::set_transfer_encoding_chunked(bool is_chunked)
{
    is_transfer_encoding_chunked_ = is_chunked;
}

void    HttpBody::AppendToBody(std::string& message)
{
    if (required_length_ > 0) {
        size_t  initial_body_length = body_.length();
        size_t  bytes_to_add = required_length_ - initial_body_length;
        size_t  message_length = message.length();
        if (bytes_to_add > message_length)
            bytes_to_add = message_length;
        body_ += message.substr(0, bytes_to_add);
        message.erase(0, bytes_to_add);
        if (initial_body_length + bytes_to_add == required_length_)
            is_complete_ = true;
    } else if (is_transfer_encoding_chunked_) {
        ; // TODO: implement transfer encoding
    }
}
