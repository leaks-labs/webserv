#ifndef HTTP_BODY_
# define HTTP_BODY_

# include <string>
# include <utility>

class HttpBody {
    public :
        HttpBody();
        HttpBody(const HttpBody& src);
        HttpBody&   operator=(const HttpBody& rhs);
        
        ~HttpBody();

        const std::string&  get_body() const;
        bool                IsComplete() const;
        size_t              get_size() const;

        void    set_required_length(size_t length);
        void    set_transfer_encoding_chunked(bool is_chunked);

        void    AppendToBody(std::string& message);

    private :
        std::string body_;
        size_t      required_length_;
        bool        is_transfer_encoding_chunked_;
        bool        is_complete_;
};

# endif
