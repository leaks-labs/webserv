#include "HttpResponse.hpp"

HttpResponse::HttpResponse(HttpRequest & request) : 
    request_(request)
{
    request_.get_header().Print();
    request_.get_request_line().Print();
}

HttpResponse::HttpResponse(HttpResponse const & src) :
    request_(src.get_request())
{
}

HttpResponse::~HttpResponse()
{
}

HttpRequest &HttpResponse::get_request() const
{
    return request_;
}