#include "HttpBody.hpp"

#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include "HttpCodeException.hpp"
#include "HttpRequest.hpp"

HttpBody::HttpBody()
    : is_complete_(false),
      max_body_size_(0),
      required_length_(0),
      mode_(kModeNone),
      chunk_size_(-1),
      is_trailer_(false),
      headers_(NULL)
{
}

HttpBody::HttpBody(const HttpBody& src)
{
    *this = src;
}

HttpBody&   HttpBody::operator=(const HttpBody& rhs)
{
    if (this != &rhs) {
        is_complete_ = rhs.is_complete_;
        max_body_size_ = rhs.max_body_size_;
        required_length_ = rhs.required_length_;
        mode_ = rhs.mode_;
        body_ = rhs.body_;
        buffer_ = rhs.buffer_;
        chunk_size_ = rhs.chunk_size_;
        is_trailer_ = rhs.is_trailer_;
        headers_ = rhs.headers_;
    }
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

void    HttpBody::set_body(const std::string& str)
{
    body_ = str;
}

void    HttpBody::set_is_complete(bool is_complete)
{
    is_complete_ = is_complete;
}

void    HttpBody::set_is_trailer(bool is_trailer)
{
    is_trailer_ = is_trailer;
}

void    HttpBody::Parse(int mode, std::string& message, HttpHeader* headers)
{
    if (headers != NULL && headers_ == NULL)
        headers_ = headers;
    if (mode_ == kModeTransferEncodingChunked)
        ParseTransferEncoding(mode, message);
    else if (mode_ == kModeContentLength)
        ParseContentLength(message);
    else
        throw std::invalid_argument("Invalid mode");
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
    if (mode != kModeContentLength && mode != kModeTransferEncodingChunked)
        throw std::invalid_argument("Invalid mode");
    max_body_size_ = max_body_size;
    mode_ = mode;
    if (mode == kModeContentLength)
        required_length_ = content_length;
}

void    HttpBody::Clear()
{
    is_complete_ = false;
    max_body_size_ = 0;
    required_length_ = 0;
    mode_ = kModeNone;
    body_.clear();
    buffer_.clear();
    chunk_size_ = -1;
    is_trailer_ = false;
    headers_ = NULL;
}

void    HttpBody::ParseContentLength(std::string& message)
{
        if (required_length_ == 0) {
            is_complete_ = true;
            return;
        }
        size_t  initial_body_length = body_.length();
        size_t  missing_bytes = required_length_ - initial_body_length;
        body_ += message.substr(0, missing_bytes);
        message.erase(0, missing_bytes);
        if (max_body_size_ > 0 && body_.length() > max_body_size_)
            throw HttpCodeExceptions::ContentTooLargeException();
        is_complete_ = (body_.length() == required_length_);
}

void    HttpBody::ParseTransferEncoding(int mode, std::string& message)
{
    while (!message.empty() && !is_complete_) {
        if (chunk_size_ == -1)
            ParseChunkSize(mode, message);
        if (!message.empty() && chunk_size_ != -1 && (chunk_size_ > 0 || !is_trailer_))
            ParseChunk(mode, message);
        else if (!message.empty() && chunk_size_ == 0 && is_trailer_)
            ParseTrailer(mode, message);
    }
    if (is_complete_ && headers_ != NULL) {
        headers_->DelOneHeader("TRANSFER-ENCODING");
        headers_->DelOneHeader("TRAILER");
        std::ostringstream ss;
        ss << body_.length();
        std::string content_length = ss.str();
        headers_->AddOneHeader("CONTENT-LENGTH", content_length);
    }
}

void    HttpBody::ParseChunkSize(int mode, std::string& message)
{

    size_t  initial_buffer_length = buffer_.length();
    buffer_ += message;
    size_t  pos = buffer_.find("\r\n");
    if (pos == std::string::npos) {
        message.clear();
        if (buffer_.length() > 10)
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
    } else {
        buffer_.erase(pos);
        size_t  bytes_to_trim_in_message = buffer_.length() - initial_buffer_length + 2;
        message.erase(0, bytes_to_trim_in_message);
        if (buffer_.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
        chunk_size_ = strtol(buffer_.c_str(), NULL, 16);
        if (chunk_size_ == 0 && buffer_.find_first_not_of('0') != std::string::npos)
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
        buffer_.clear();
        if (max_body_size_ > 0 && body_.length() + chunk_size_ > max_body_size_)
            throw HttpCodeExceptions::ContentTooLargeException();
    }
}

void    HttpBody::ParseChunk(int mode, std::string& message)
{

    size_t  bytes_to_add = chunk_size_ - buffer_.length() + 2;
    buffer_ += message.substr(0, bytes_to_add);
    message.erase(0, bytes_to_add);
    if (buffer_.length() == static_cast<size_t>(chunk_size_ + 2)) {
        size_t  pos = buffer_.rfind("\r\n");
        if (pos == std::string::npos || pos != static_cast<size_t>(chunk_size_))
            mode == kParseRequest ? throw HttpCodeExceptions::BadRequestException() : throw HttpCodeExceptions::BadGatewayException();
        body_ += buffer_.substr(0, chunk_size_);
        buffer_.clear();
        if (max_body_size_ > 0 && body_.length() > max_body_size_)
            throw HttpCodeExceptions::ContentTooLargeException();
        if (chunk_size_ == 0)
            is_complete_ = true;
        chunk_size_ = -1;
    }
}

void    HttpBody::ParseTrailer(int mode, std::string& message)
{
    if (headers_ == NULL)
        throw HttpCodeExceptions::InternalServerErrorException();
    int mode_header = mode == kParseRequest ? HttpHeader::kParseRequest : HttpHeader::kParseResponse;
    if (headers_->ParseTrailer(message, mode_header) == HttpHeader::kHeaderParsed)
        is_complete_ = true;
}
