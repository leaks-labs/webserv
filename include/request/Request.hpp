#ifndef REQUEST_HPP_
# define REQUEST_HPP_

# include "RequestLine.hpp"
# include "RequestHeader.hpp"

# include <string>

# define INPUT_ERROR "Reading Error: Failed to read input file\n"

class Request
{
    public:
        /* Constructor for Request class */
        Request();

        /* Destructor for Request class */
        ~Request();

        /* Getter for request_line_ attribute */
        const RequestLine& GetRequestLine() const;

        /* Getter for request_header_ attribute */
        const RequestHeader& GetRequestHeader() const;

        /* Getter for request_raw_ attribute */
        const std::string& GetRequestRaw() const;

        /* Getter for is_request_complete_ attribute */
        bool GetIsRequestComplete() const;

        /* AddData() to fill request line till it's complete and then set
         * is_compete_ to true otherwise set is_complete_ to false*/

        /* Parse and verify request method, can't parse if request
         * is incomplete*/
        void Parse();

    private:
        /* Private class handling event, should not be used */
        Request(const Request& request);
        Request& operator=(const Request& request);

        /* Getter for request_line_ attribute (non const for full access) */
        RequestLine& GetRequestLinePrivate();

        /* Getter for request_header_ attribute (non const for full access) */
        RequestHeader& GetRequestHeaderPrivate();

        /* Setter for is_complete_ attribute (only accessible by definition) */
        void SetIsComplete(bool is_request_complete);

        /* Private attribute for Request class */
        RequestLine         request_line_;
        RequestHeader       request_header_;
        std::string         request_raw_;
        bool                is_request_complete_;
};

#endif