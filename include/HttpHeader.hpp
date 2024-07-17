#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <string>
# include <utility>

class HttpHeader {
    public:
        static const int    kParseRequest = 1;
        static const int    kParseResponse = 2;

        HttpHeader();
        HttpHeader(const HttpHeader& src);
        HttpHeader& operator=(const HttpHeader& rhs);

        ~HttpHeader();

        const std::map<std::string, std::string>&   get_header_map() const;
        void                                        set_host(const std::string& host);

        void        Parse(std::string& message, int mode);
        bool        IsComplete() const;
        bool        NeedBody() const;
        bool        IsContentLength() const;
        bool        BodyIsTransferChunked() const;
        size_t      GetContentLength() const;
        std::string GetFormatedHeader() const;
        void        AddOneHeader(const std::string& key, const std::string& value);
        void        Clear();
        void        Print() const;

    private:
        static const int    kMaxHeaderSize = 32768;

        static std::pair<std::string, std::string>  ParseOneLine(const std::string& line);
        void                                        ToLower(std::string& str);

        bool                                is_complete_;
        bool                                need_body_;
        std::map<std::string, std::string>  header_map_;
        std::string                         buffer_;
};

#endif
