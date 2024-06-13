#ifndef REQUEST_HPP_
# define REQUEST_HPP_

# include "RequestLine.hpp"
# include "RequestHeader.hpp"

# include <string>

class Request
{
    public:
        /* Constructor for Request class */
        explicit Request(const std::string& request_raw);

        /* Destructor for Request class */
        ~Request();

        /* Getter for request_raw_ attribute */
        const std::string& GetRequestRaw() const;

        /* Parse and verify request method */
        void Parse();

    private:
        /* Private class handling event, should not be used */
        Request();
        Request(const Request& request);
        Request& operator=(const Request& request);

        /* Private attribute for Request class */
        const std::string   request_raw_;
        RequestLine         request_line_;
        RequestHeader       request_header_;
};

#endif