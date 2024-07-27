#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <string>
# include <utility>
# include <vector>

class HttpHeader {
    public:
        static const int    kParseRequest = 1;
        static const int    kParseResponse = 2;

        static const bool   kHeaderNotParsed = false;
        static const bool   kHeaderParsed = true;

        HttpHeader();
        HttpHeader(const HttpHeader& src);
        HttpHeader& operator=(const HttpHeader& rhs);

        ~HttpHeader();

        const std::map<std::string, std::string>&   get_header_map() const;

        void        Parse(std::string& message, int mode);
        bool        ParseTrailer(std::string& message, int mode);
        bool        IsComplete() const;
        bool        NeedBody() const;
        bool        IsContentLength() const;
        bool        BodyIsTransferChunked() const;
        bool        IsTrailer() const;
        size_t      GetContentLength() const;
        std::string GetFormatedHeader() const;
        void        AddOneHeader(const std::string& key, const std::string& value);
        void        DelOneHeader(const std::string& key);
        void        Clear();
        void        Print() const;

    private:
        typedef void (HttpHeader::*MemberFunctionPtr)(std::string&, int mode);

        static const int    kMaxHeaderSize = 32768;

        static const std::map<std::string, MemberFunctionPtr>   specific_header_handling_functions_;
        static const std::map<std::string, MemberFunctionPtr>   InitSpecificHeaderHandlingFuctions();

        static std::pair<std::string, std::string>  ParseOneLine(const std::string& line, int mode);
        static void                                 ToLower(std::string& str);
        static void                                 ToUpper(std::string& str);

        bool    DivideIntoTokens(std::string& message, std::vector<std::string>& tokens, int ode);
        void    HandleTokens(std::vector<std::string>& tokens, int mode);
        void    HandleTrailerTokens(std::vector<std::string>& tokens, int ode);
        void    HandleContentLength(std::string& value, int mode);
        void    HandleTransferEncoding(std::string& value, int mode);
        void    HandleConnection(std::string& value, int mode);
        void    HandleTrailer(std::string& value, int mode);
        void    AdditionalCheck(int mode) const;
        void    AdditionalTrailerCheck(int mode) const;

        bool                                is_complete_;
        bool                                need_body_;
        std::map<std::string, std::string>  header_map_;
        std::string                         buffer_;
};

#endif
