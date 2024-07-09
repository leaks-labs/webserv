#ifndef HTTP_BODY_
# define HTTP_BODY_

# include <string>
//# include "HttpMessage.hpp"
//# include "HttpRequest.hpp"

class HttpBody
{
    public :
        HttpBody();
        ~HttpBody();

        const std::string&  get_body() const;
        void                set_body(const std::string& body);
        bool                get_is_body() const;
        void                set_is_body(bool is_complete);
        bool                get_is_complete() const;
        void                set_is_complete(bool is_complete);

        const std::pair<std::string, std::string>&  get_transfer_type() const;
        void                                        set_transfer_type(
                const std::pair<std::string, std::string>& transfer_type
        );
        //bool                SearchBody(const HttpRequest& request);
        size_t Transfer(
                std::pair<std::string, std::string> transfer_type,
                const std::string& body
        );

    private :
        HttpBody(const HttpBody& httpBody);
        HttpBody& operator=(const HttpBody& httpBody);

        size_t TransferLength(const std::string& body, size_t bytes);
        //size_t Transfer(const std::string body);

        std::pair<std::string, std::string> transfer_type_;
        std::string                         body_;
        bool                                is_body_;
        bool                                is_complete_;
};

# endif