#ifndef HTTP_BODY_
# define HTTP_BODY_

# include <string>

class HttpBody
{
public :
    HttpBody();
    ~HttpBody();

    const std::pair<std::string, std::string>&  get_transfer_type() const;
    void                                        set_transfer_type(
            const std::pair<std::string, std::string>& transfer_type
    );
    const std::string&  get_body() const;
    void                set_body(const std::string &body, size_t pos);
    bool                get_is_complete() const;
    void                set_is_complete(bool is_complete);

    bool                SearchBody(const HttpMessage& request);

private :
    std::pair<std::string, std::string> transfer_type_;
    std::string body_;
    bool is_body_;
    bool is_complete_;
};

# endif