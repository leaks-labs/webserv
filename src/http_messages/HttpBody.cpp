#include "HttpBody.hpp"

#include <cstdlib>
#include <stdexcept>

HttpBody::HttpBody()
    : is_complete_(false),
      max_body_size_(kMaxBodySize),
      required_length_(0),
      is_transfer_encoding_chunked_(false)
{
}

HttpBody::HttpBody(const HttpBody& src)
{
    *this = src;
}

HttpBody&   HttpBody::operator=(const HttpBody& rhs)
{
    if (this != &rhs)
        is_complete_ = rhs.is_complete_;
        max_body_size_ = rhs.max_body_size_;
        required_length_ = rhs.required_length_;
        is_transfer_encoding_chunked_ = rhs.is_transfer_encoding_chunked_;
        body_ = rhs.body_;
    return *this;
}

HttpBody::~HttpBody()
{
}

const std::string&  HttpBody::get_body() const
{
    return body_;
}

std::string&    HttpBody::get_body()
{
    return body_;
}

void    HttpBody::Parse(std::string& message)
{
    if (!is_transfer_encoding_chunked_) {
        if (required_length_ == 0) {
            is_complete_ = true;
            return;
        }
        size_t  initial_body_length = body_.length();
        size_t  missing_bytes = required_length_ - initial_body_length;
        body_ += message.substr(0, missing_bytes);
        if (body_.length() > max_body_size_)
            throw std::runtime_error("413");
        message.erase(0, missing_bytes);
        is_complete_ = (body_.length() == required_length_);
    } else {
        ; // TODO: implement transfer encoding
    }
}

bool    HttpBody::IsComplete() const
{
    return is_complete_;
}

size_t  HttpBody::Size() const
{
    return body_.length();
}

void    HttpBody::SetMode(int mode, size_t max_body_size, size_t content_length)
{
    max_body_size_ = max_body_size;
    if (mode == kModeContentLength)
        required_length_ = content_length;
    else if (mode == kModeTransferEncodingChunked)
        is_transfer_encoding_chunked_ = true;
    else
        throw std::invalid_argument("Invalid mode");
}
