#include <stdexcept>
#include <cstdlib>
#include "HttpBody.hpp"
//#include "HttpMessage.hpp"

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

void HttpBody::set_body(const std::string &body)
{
    body_ = body;
}

bool HttpBody::get_is_body() const
{
    return is_body_;
}

void HttpBody::set_is_body(bool is_body)
{
    is_body_ = is_body;
}

bool HttpBody::get_is_complete() const
{
    return is_complete_;
}

void HttpBody::set_is_complete(bool is_complete)
{
    is_complete_ = is_complete;
}

size_t HttpBody::Transfer(
        std::pair<std::string, std::string> transfer_type,
        const std::string &body)
{
    size_t bytes = 0;
    if (transfer_type.first == "CONTENT-LENGTH")
        bytes = TransferLength(body, std::atoi(transfer_type.second.c_str()));
    //else
    //Transfer(body);
    return bytes;
}

size_t HttpBody::TransferLength(const std::string& body, size_t bytes)
{
    if (body.length() != bytes)
        throw std::runtime_error("Bad body length");
    set_body(body);
    set_is_complete(true);
    return bytes;
}

//size_t HttpBody::Transfer(const std::string &body)
//{
//    static size_t previous_body_length;
//    set_body(get_body() + body);
//    previous_body_length = body.length();
//    return previous_body_length;
//}
//bool HttpBody::SearchBody(const HttpRequest& request)
//{
//    int factor = 0;
//    if (request.get_request_line().get_method().first == "POST" ||
//        request.get_request_line().get_method().first == "PUT" ||
//        request.get_request_line().get_method().first == "DELETE")
//    {
//        factor++;
//    }
//    std::map<std::string, std::string>::const_iterator it1 = request.get_header().get_header_map().find("Content-Length");
//    if (it1 != request.get_header().get_header_map().end() && it1->first == "Content-Length")
//    {
//        set_transfer_type(*it1);
//        factor++;
//    }
//    else
//    {
//        std::map<std::string, std::string>::const_iterator it2 = request.get_header().get_header_map().find("Transfer-Encoding");
//        if (it2 != request.get_header().get_header_map().end() && it2->first == "Transfer-Encoding")
//        {
//            set_transfer_type(*it2);
//            factor++;
//        }
//    }
//    return factor == 2;
//    return false;
//}
