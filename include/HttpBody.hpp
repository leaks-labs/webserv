#ifndef HTTP_BODY_
# define HTTP_BODY_

# include <map>
# include <string>
# include <utility>
# include <stdint.h>
# include "HttpHeader.hpp"

class HttpBody {
    public :
        static const int   kModeNone = 0;
        static const int   kModeContentLength = 1;
        static const int   kModeTransferEncodingChunked = 2;

        static const int   kParseRequest = 1;
        static const int   kParseResponse = 2;

        HttpBody();
        HttpBody(const HttpBody& src);
        HttpBody&   operator=(const HttpBody& rhs);
        
        ~HttpBody();

        const std::string&  get_body() const;
        std::string&        get_body();

        void    set_body(const std::string& str);
        void    set_is_complete(bool is_complete);
        void    set_is_trailer(bool is_trailer);

        void    Parse(int mode, std::string& message, HttpHeader* headers = NULL);
        bool    IsComplete() const;
        size_t  Size() const;
        void    SetMode(int mode, size_t max_body_size, size_t content_legnth = 0);
        void    Clear();

    private :
        void    ParseContentLength(std::string& message);
        void    ParseTransferEncoding(int mode, std::string& message);
        void    ParseChunkSize(int mode, std::string& message);
        void    ParseChunk(int mode, std::string& message);
        void    ParseTrailer(int mode, std::string& message);

        bool        is_complete_;
        size_t      max_body_size_;
        size_t      required_length_;
        int         mode_;
        std::string body_;
        std::string buffer_;
        int64_t     chunk_size_;
        bool        is_trailer_;
        HttpHeader* headers_;
};

# endif
