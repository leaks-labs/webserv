#ifndef HTTP_MESSAGE_HPP_
# define HTTP_MESSAGE_HPP_

# include "HttpHeader.hpp"
# include "HttpRequestLine.hpp"

# include <map>
# include <string>



class HttpMessage
{
    public:
            ;
    private:
        HttpRequestLine             request_line_;
        HttpHeader                  message_header_;
        std::map<int, std::string>  message_status_;
        std::string                 message_;
        std::string                 body_;
        bool                        is_complete_;

};

#endif