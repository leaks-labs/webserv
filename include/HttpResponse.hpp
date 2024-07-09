#ifndef HTTP_RESPONSE_HPP_
# define HTTP_RESPONSE_HPP_

# include "HttpMessage.hpp"
# include "HttpRequest.hpp"

class HttpResponse
{
    public :
        HttpResponse(HttpRequest & request);
        HttpResponse(HttpResponse const & src);
        HttpRequest &get_request() const;
        ~HttpResponse();

    private:
        HttpResponse();
        HttpResponse& operator=(HttpResponse const & src);

        HttpRequest & request_;
};

#endif