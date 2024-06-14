#ifndef REQUEST_LINE_HPP_
# define REQUEST_LINE_HPP_

# include <string>

/* default HTTP version for the webserver, if not matched, throw an error */
/* Maybe should be in WebServ.hpp */
# define HTTP_VERSION "1.1"

/* Enum to represent url types */
enum e_request_url_type {
    URL,
    PATH,
    AUTHORITY
};

/* Struct to store the url and his type */
struct s_request_uri {
    std::string         uri;
    e_request_url_type  uri_type;
};

/* Struct to build a dictionary of available request type */
struct s_request_dictionary {
    std::string request_type;
    bool        is_used;
};

const s_request_dictionary request_dictionary[] = {
        {"OPTIONS", false},
        {"GET", true},
        {"HEAD", false},
        {"POST", true},
        {"PUT", false},
        {"DELETE", true},
        {"TRACE", false},
        {"CONNECT", false},
};

class RequestLine
{
    public:
        /* Constructor for RequestLine class */
        RequestLine();

        /* Destructor for RequestLine class */
        ~RequestLine();

        /* Copy constructor for RequestLine class */
        RequestLine(const RequestLine& requestLine);

        /* Assignment constructor for RequestLine class */
        RequestLine& operator=(const RequestLine& requestLine);

        /* Getter for request_type_ attribute */
        const std::string& GetRequestType() const;

        /* Setter for request_type_ attribute */
        void SetRequestType(const std::string& request_type);

        /* Getter for request_uri_ attribute */
        const s_request_uri& GetRequestUri() const;

        /* Setter for request_uri_ attribute */
        void SetRequestUri(const s_request_uri& request_uri);

        /* Getter for request_http_version_ attribute */
        const std::string& GetRequestHttpVersion() const;

        /* Setter for request_http_version_ attribute */
        void SetRequestHttpVersion(const std::string& request_http_version);

        /* Find matching request type and return true if a match is found
         * and available for use, otherwise return false */
        static bool FindDictionaryRequest();

        /* Parse and verify request method */
        void Parse(const std::string& request_line);

    private:
        /* Class RequestLine private attribute */
        s_request_uri           request_uri_;
        std::string             request_type_;
        std::string             request_http_version_;
};

#endif