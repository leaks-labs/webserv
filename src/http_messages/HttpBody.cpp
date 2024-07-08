#include "HttpBody.hpp"

HttpBody::HttpBody()
        : is_body_(false), is_complete_(false)
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

bool HttpBody::SearchBody(const HttpMessage& request)
{
    int factor = 0;
    if (request.get_request_line().get_method().first == "POST" ||
        request.get_request_line().get_method().first == "PUT" ||
        request.get_request_line().get_method().first == "DELETE")
    {
        factor++;
    }
    std::map<std::string, std::string>::const_iterator it1 = request.get_header().get_header_map().find("Content-Length");
    if (it1 != request.get_header().get_header_map().end() && it1->first == "Content-Length")
    {
        set_transfer_type(*it1);
        factor++;
    }
    else
    {
        std::map<std::string, std::string>::const_iterator it2 = request.get_header().get_header_map().find("Transfer-Encoding");
        if (it2 != request.get_header().get_header_map().end() && it2->first == "Transfer-Encoding")
        {
            set_transfer_type(*it2);
            factor++;
        }
    }
    return factor == 2;
}
