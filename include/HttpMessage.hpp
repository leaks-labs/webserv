#ifndef HTTP_MESSAGE_HPP_
# define HTTP_MESSAGE_HPP_

# include "HttpHeader.hpp"
# include "HttpRequestLine.hpp"
# include "HttpBody.hpp"

# include <map>
# include <string>

class HttpMessage
{
    public:
        HttpMessage();
        virtual ~HttpMessage();

        static const std::map<std::string, std::string> status_map;

        static void Split(
                const std::string& str,
                const std::string& delim,
                std::vector<std::string>& tokens
        );

        const HttpRequestLine&                      get_request_line() const;
        const HttpHeader&                           get_header() const;
        const HttpBody&                             get_body() const;

        const std::pair<std::string, std::string>&  get_status() const;
        const std::string&                          get_message() const;
        bool                                        get_is_complete() const;

        void    set_status(const std::pair<std::string, std::string>& status);
        void    set_message(const std::string& message);
        void    set_is_complete(bool is_complete);

    protected:
        HttpRequestLine&    get_protected_request_line();
        HttpHeader&         get_protected_header();
        HttpBody&           get_protected_body();

    private:
        HttpMessage(const HttpMessage& http_message);
        HttpMessage& operator=(const HttpMessage& http_message);

        static std::map<std::string, std::string> init_status_map();

        HttpRequestLine                     request_line_;
        HttpHeader                          header_;
        HttpBody                            body_;
        std::pair<std::string, std::string> status_;
        std::string                         message_;
        bool                                is_complete_;
};

#endif