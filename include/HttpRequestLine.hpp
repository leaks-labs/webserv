#ifndef HTTP_REQUEST_LINE_HPP_
# define HTTP_REQUEST_LINE_HPP_

# include <string>

# define HTTP_VERSION "HTTP/1.1"
# define N_METHOD 8

enum RequestTargetType {
    ORIGIN_FORM,
    ABSOLUTE_FORM,
    AUTHORITY_FORM,
    ASTERISK_FORM,
    UNKNOWN_FORM
};

struct RequestTarget {
    std::string         target;
    RequestTargetType   type;
};

struct RequestMethod {
    std::string method;
    size_t      len;
    bool        is_used;
};

extern const RequestMethod method_dictionary[N_METHOD];

class HttpRequestLine
{
    public:
        HttpRequestLine();
        ~HttpRequestLine();

        const RequestMethod& get_request_method() const;
        const RequestTarget& get_request_target() const;
        const std::string& get_request_http() const;
        void Parse(const std::string& request_line);
        static size_t FindMethod(
                    const RequestMethod* dictionary,
                    const std::string& request_method
        );
        static

    private:
        void set_request_method(const RequestMethod& request_method);
        void set_request_target(const RequestTarget& request_target);
        void set_request_http(const std::string& request_http);

        RequestMethod   request_method_;
        RequestTarget   request_target_;
        std::string     request_http_;
};

# endif