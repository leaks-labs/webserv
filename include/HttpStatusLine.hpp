#ifndef HTTP_STATUS_LINE_HPP
# define HTTP_STATUS_LINE_HPP

# include <string>
# include <vector>
# include <map>

class HttpStatusLine {
    public:
        static const std::map<int, std::string>&    get_status_code_map();

        HttpStatusLine();
        HttpStatusLine(int code);
        HttpStatusLine(const HttpStatusLine& src);
        HttpStatusLine& operator=(const HttpStatusLine& rhs);
        
        ~HttpStatusLine();

        const std::string&                  get_http_version() const;
        std::string                         get_status_code_str() const;
        int                                 get_status_code() const;
        const std::string&                  get_reason_phrase() const;
        const std::vector<int>&             get_codes_requiring_close() const;

        void        Parse(std::string& message);
        void        SetCodeAndPhrase(int code);
        bool        IsComplete() const;
        std::string GetFormatedStatusLine() const;
        void        Clear();
        void        Print() const;

    private:
        static const std::vector<int>           codes_requiring_close_;
        static const std::map<int, std::string> status_code_map_;

        static const std::vector<int>           InitCodesRequiringClose();
        static const std::map<int, std::string> InitStatusCodeMap();
        static size_t                           FindEndOfStatusLine(const std::string& buff);

        static const int    kNotFoundEnd = 0;
        static const int    kTerminatorSize = 2;

        bool        is_complete_;
        std::string http_version_;
        int         status_code_;
        std::string reason_phrase_;
        std::string buffer_;
};

#endif  // HTTP_STATUS_LINE_HPP
