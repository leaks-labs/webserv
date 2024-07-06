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

        /* Copy constructor for Request class */
        Request(const Request& request);

        /* Assignment constructor for Request class */
        Request& operator=(const Request& request);

        /* Getter for request_line_ attribute */
        const RequestLine& GetRequestLine() const;

        /* Getter for request_header_ attribute */
        const RequestHeader& GetRequestHeader() const;

        /* Getter for request_ attribute */
        const std::string& GetRequest() const;

        /* Getter for is_request_complete_ attribute */
        bool GetIsRequestComplete() const;

        /* Fill request_ till it's complete (checked by IsRequestComplete())
         * and then set is_request_compete_ to true */
        void AddRequestData(const std::string& request_data);

        /* Check if request_ is complete, return true for complete, false for
         * incomplete */
        bool IsRequestComplete(const std::string& request) const;

        /* Parse and verify request method, can't parse if request
         * is incomplete*/
        void Parse();

    private:
        /* Getter for request_line_ attribute (non const for full access) */
        RequestLine& GetRequestLinePrivate();

        /* Getter for request_header_ attribute (non const for full access) */
        RequestHeader& GetRequestHeaderPrivate();

        /* Setter for request_ attribute (only accessible by definition) */
        void SetRequest(const std::string& request);

        /* Setter for is_complete_ attribute (only accessible by definition) */
        void SetIsRequestComplete(bool is_request_complete);

        /* Private attribute for Request class */
        RequestLine         request_line_;
        RequestHeader       request_header_;
        std::string         request_;
        std::string         body_;
        bool                is_request_complete_;
};

#endif