#ifndef HTTP_REQUEST_LINE_HPP_
# define HTTP_REQUEST_LINE_HPP_

# include <string>
# include <map>

# define ORIGIN_FORM "/"
# define ABSOLUTE_FORM "http://"
# define AUTHORITY_FORM "//"
# define ASTERISK_FORM "*"

struct Target {
    std::pair<std::string, bool> type;
    std::string                 target;
    bool                        is_cgi;
};

class HttpRequestLine
{
    public:
        HttpRequestLine();
        ~HttpRequestLine();

        static const std::map<std::string, bool>    method_map;
        static const std::map<std::string, bool>    target_map;

        const std::pair<std::string, bool>& get_method() const;
        const Target&                       get_target() const;
        const std::string&                  get_http_version() const;

        void                         Parse(const std::string& request_line);
        static std::map<std::string, bool>::const_iterator InitTargetType(
                const std::string& target
        );

    private:
        HttpRequestLine(const HttpRequestLine& request_line);
        HttpRequestLine& operator=(const HttpRequestLine& request_line);

        static std::map<std::string, bool>  InitMethodMap();
        static std::map<std::string, bool>  InitTargetMap();

        void set_method(const std::pair<std::string, bool>& method);
        void set_target(const Target& target);
        void set_http_version(const std::string& http_version);

        std::pair<std::string, bool>    method_;
        Target                          target_;
        std::string                     http_version_;
        std::string                     line_;
};

# endif