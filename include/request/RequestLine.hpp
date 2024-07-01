#ifndef REQUEST_LINE_HPP_
# define REQUEST_LINE_HPP_

# include <string>

/* default HTTP version for the webserver */
# define HTTP_VERSION "1.1" //enum

/* Enum to represent request target type */
enum RequestTargetType {
    ORIGIN_FORM,
    ABSOLUTE_FORM,
    AUTHORITY_FORM,
    ASTERISK_FORM
};

/* Struct to store the request target and his type */
struct RequestTarget {
    std::string         target;
    RequestTargetType   type;
};

/* Struct to store request, request len and request availability */
struct RequestMethod {
    std::string method;
    size_t      len;
    bool        is_used;
};

/* Dictionary to find available request method */
const RequestMethod method_dictionary[] = {
    {"OPTIONS", 7, false},
    {"GET", 3, true},
    {"HEAD", 4, false},
    {"POST", 4,true},
    {"PUT", 3, false},
    {"DELETE", 6, true},
    {"TRACE", 5, false},
    {"CONNECT", 7, false},
};

class RequestLine
{
    public:
        /* Constructor for RequestLine class */
        RequestLine();

        /* Destructor for RequestLine class */
        ~RequestLine();

        /* Copy constructor for RequestLine class */
        RequestLine(const RequestLine& request_line);

        /* Assignment constructor for RequestLine class */
        RequestLine& operator=(const RequestLine& request_line);

        /* Getter for request_target_ attribute */
        const RequestTarget& get_request_target() const;

        /* Getter for request_method_ attribute */
        const std::string& get_request_method() const;

        /* Getter for request_http_version_ attribute */
        const std::string& get_request_http_version() const;

        /* Find matching request type and return true if a match is found
         * and available for use, otherwise return false */
        static bool FindDictionaryRequest(
            RequestMethod method_dictionary[],
            const std::string& request_method
        );

        /* Parse and verify request method */
        void Parse(const std::string& request_line);

    private:
        /* Setter for request_target_ attribute (only accessible by definition) */
        void set_request_target(const RequestTarget& request_target);

        /* Setter for request_method_ attribute (only accessible by definition) */
        void set_request_method(const std::string& request_method);

        /* Setter for request_http_version_ attribute
         * (only accessible by definition) */
        void set_request_http_version(const std::string& request_http_version);

        /* Class RequestLine private attribute */
        RequestTarget   request_target_;
        /* A server that receives a method longer than any that it implements
           SHOULD respond with a 501 (Not Implemented) status code.
        An origin server that receives a request method that is recognized
         and implemented, but not allowed for the target resource,
         SHOULD respond with the 405 (Method Not Allowed) status code */
        RequestMethod   request_method_;
        /* A server that receives a request-target longer than any URI it
         * wishes to parse MUST respond with a 414 (URI Too Long) status code */
        std::string     request_http_version_;
};

#endif