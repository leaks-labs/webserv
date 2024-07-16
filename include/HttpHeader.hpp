#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <string>
# include <utility>

class HttpHeader {
    public:
        HttpHeader();
        HttpHeader(const HttpHeader& src);
        HttpHeader& operator=(const HttpHeader& rhs);

        ~HttpHeader();

        const std::map<std::string, std::string>&   get_header_map() const;
        void                                        set_host(const std::string& host);

        void        Parse(std::string& message);
        bool        IsComplete() const;
        bool        NeedBody() const;
        bool        IsContentLength() const;
        bool        BodyIsTransferChunked() const;
        size_t      GetContentLength() const;
        std::string GetFormatedHeader() const;
        void        Print() const;

    private:
        static const size_t kNotFoundEnd = 0;
        static const size_t kTerminatorSize = 4;

        static size_t                               FindEndOfHeader(const std::string& buff);
        static std::pair<std::string, std::string>  ParseOneLine(const std::string& line);

        bool                                is_complete_;
        bool                                need_body_;
        std::map<std::string, std::string>  header_map_;
        std::string                         buffer_;
};

#endif
