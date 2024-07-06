#ifndef HTTP_MESSAGE_HPP_
# define HTTP_MESSAGE_HPP_

# include "HttpHeader.hpp"
# include "HttpRequestLine.hpp"

# include <map>
# include <string>

# define N_STATUS 10

struct MessageStatus
{
    std::string message;
    int         status;
};

extern const MessageStatus status_dictionary[N_STATUS];

class HttpMessage
{
    public:
        HttpMessage();
        virtual ~HttpMessage();

        static size_t FindStatus(const MessageStatus* dictionary, int status);

        const HttpRequestLine&  get_request_line() const;
        const HttpHeader&       get_message_header() const;

        const MessageStatus&    get_message_status() const;
        void                    set_message_status(const MessageStatus& message_status);
        const std::string&      get_message() const;
        void                    set_message(const std::string& message);
        const std::string&      get_body() const;
        void                    set_body(const std::string& body);
        bool                    get_is_chunk() const;
        void                    set_is_chunk(bool is_chunk);
        bool                    get_is_complete() const;
        void                    set_is_complete(bool is_complete);

    protected:
        HttpRequestLine&    get_protected_request_line();
        HttpHeader&         get_protected_message_header();

    private:
        HttpMessage(const HttpMessage& http_message);
        HttpMessage& operator=(const HttpMessage& http_message);

        HttpRequestLine request_line_;
        HttpHeader      message_header_;
        MessageStatus   message_status_;
        std::string     message_;
        std::string     body_; // set struct for body(len/chunk)
        bool            is_chunk_;
        bool            is_complete_;
};

#endif