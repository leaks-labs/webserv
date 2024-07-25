#ifndef HTTP_REQUEST_LINE_HPP_
# define HTTP_REQUEST_LINE_HPP_

# include <map>
# include <string>

class HttpRequestLine {
    public:
        class Target {
            public:
                static std::string  UrlCleaner(const std::string& url);

                Target();
                Target(const Target& src);
                Target& operator=(const Target& rhs);

                ~Target();

                const std::string&  get_type() const;
                const std::string&  get_target() const;
                const std::string&  get_query() const;
                const std::string&  get_fragment() const;
                const std::string&  get_complete_url() const;

                void    set_type(const std::string& type);
                void    set_target(const std::string& target);
                void    set_query(const std::string& query);
                void    set_fragment(const std::string& fragment);
                void    set_complete_url(const std::string& url);

                void    InitTargetType(const std::string& target);
                bool    IsTargetEmpty() const;
                void    ClearTarget();

            private:
                static const std::string    kOriginForm;
                static const std::string    kAbsoluteForm;
                static const std::string    kAsteriskForm;

                static const std::map<std::string, bool>    target_map;

                static std::map<std::string, bool>                  InitTargetMap();

                void    UpdateCompleteUrl();

                std::string type_;
                std::string target_;
                std::string query_;
                std::string fragment_;
                std::string complete_url;
        };

        static const std::map<std::string, bool>    method_map;

        HttpRequestLine();
        HttpRequestLine(const HttpRequestLine& src);
        HttpRequestLine& operator=(const HttpRequestLine& rhs);

        ~HttpRequestLine();

        const std::string&  get_method() const;
        const Target&       get_target() const;
        Target&             get_target();
        const std::string&  get_http_version() const;

        void                set_method(std::string value);
        void                Parse(std::string& message);
        bool                IsComplete() const;
        std::string         GetFormatedRequestLine() const;
        void                Print() const;

    private:
        static const int    kMaxRequestLineSize = 8192;

        static std::map<std::string, bool>  InitMethodMap();

        bool        is_complete_;
        std::string method_;
        Target      target_;
        std::string http_version_;
        std::string buffer_;
};

# endif
