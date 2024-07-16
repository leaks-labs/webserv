#ifndef HTTP_BODY_
# define HTTP_BODY_

# include <string>
# include <utility>

class HttpBody {
    public :
        static const int   kModeContentLength = 1;
        static const int   kModeTransferEncodingChunked = 2;

        HttpBody();
        HttpBody(const HttpBody& src);
        HttpBody&   operator=(const HttpBody& rhs);
        
        ~HttpBody();

        const std::string&  get_body() const;
        std::string&        get_body();

        void    set_body(const std::string& str);

        void    Parse(std::string& message);
        bool    IsComplete() const;
        size_t  Size() const;
        void    SetMode(int mode, size_t max_body_size = kMaxBodySize, size_t content_legnth = 0);
        void    Clear();

    private :
        static const int    kMaxBodySize = 1024;

        bool        is_complete_;
        size_t      max_body_size_;
        size_t      required_length_;
        bool        is_transfer_encoding_chunked_;
        std::string body_;
};

# endif
