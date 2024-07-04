#ifndef HTTP_REQUEST_HPP_
# define HTTP_REQUEST_HPP_

# include "HttpMessage.hpp"

class HttpRequest : public HttpMessage
{
    public :
        HttpRequest();
        ~HttpRequest();

        static size_t   FindRequest(const std::string& buff, size_t pos);

        void            Parse();
    private:
        HttpRequest(const HttpRequest& http_request);
        HttpRequest& operator=(const HttpRequest& http_request);
};

#endif