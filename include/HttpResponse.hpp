#ifndef HTTP_RESPONSE_HPP_
# define HTTP_RESPONSE_HPP_

# include "HttpMessage.hpp"

class HttpResponse : HttpMessage
{
    public :
        HttpResponse();
        ~HttpResponse();

    private:
        HttpResponse(const HttpResponse& http_response);
        HttpResponse& operator=(const HttpResponse& http_response);
};

#endif