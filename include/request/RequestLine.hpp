#ifndef REQUEST_LINE_HPP_
# define REQUEST_LINE_HPP_

# include <string>

/* Enum to represent url type */
enum e_request_url_type_ {
    URL,
    PATH,
    AUTHORITY
};

/* Struct to store the url and his type */
struct s_request_url {
    std::string         url;
    e_request_url_type_ url_type;
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

        /* Getter for request_url_ attribute */
        const s_request_url& GetRequestUrl() const;

        /* Setter for request_url_ attribute */
        void SetRequestUrl(const s_request_url& request_url);

        /* Getter for request_http_version_ attribute */
        const std::string& GetRequestHttpVersion() const;

        /* Setter for request_http_version_ attribute */
        void SetRequestHttpVersion(const std::string& request_http_version);

    private:
        /* Class RequestLine private attribute */
        std::string     request_type_;
        s_request_url   request_url_;
        std::string     request_http_version_;
};

#endif