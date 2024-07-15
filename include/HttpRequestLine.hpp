#ifndef HTTP_REQUEST_LINE_HPP_
# define HTTP_REQUEST_LINE_HPP_

# include <map>
# include <string>

class HttpRequestLine {
    public:
        class Target {
            public:
                Target();
                Target(const Target& src);
                Target& operator=(const Target& rhs);

                ~Target();

                const std::string&  get_type() const;
                const std::string&  get_target() const;
                const std::string&  get_query() const;
                const std::string&  get_fragment() const;

                void    set_type(const std::string& type);
                void    set_target(const std::string& target);
                void    set_query(const std::string& query);
                void    set_fragment(const std::string& fragment);

            private:
                std::string type_;
                std::string target_;
                std::string query_;
                std::string fragment_;
        };

        static const std::string    kOriginForm;
        static const std::string    kAbsoluteForm;
        static const std::string    kAsteriskForm;

        static const std::map<std::string, bool>    method_map;
        static const std::map<std::string, bool>    target_map;

        HttpRequestLine();
        HttpRequestLine(const HttpRequestLine& src);
        HttpRequestLine& operator=(const HttpRequestLine& rhs);

        ~HttpRequestLine();

        const std::string&  get_method() const;
        const Target&       get_target() const;
        const std::string&  get_http_version() const;
        const std::string&  get_line() const;

        void        Parse(const std::string& request_line);
        std::string UrlCleaner(const std::string& url) const;
        void        Print() const;

    private:
        static std::map<std::string, bool>::const_iterator  InitTargetType(const std::string& target);
        static std::map<std::string, bool>                  InitMethodMap();
        static std::map<std::string, bool>                  InitTargetMap();

        std::string method_;
        Target      target_;
        std::string http_version_;
        std::string line_;
};

# endif
