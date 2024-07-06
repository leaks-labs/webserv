#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <vector>
# include <string>

# define N_HEADERS 10
# define N_HEADERS_ARGS 3

struct HttpHeaderList
{
    std::string header;
    std::string args[N_HEADERS_ARGS];
};

extern const HttpHeaderList header_dictionary[N_HEADERS];

class HttpHeader
{
    public:
        HttpHeader();
        ~HttpHeader();

        static size_t                               FindHeader(
                const HttpHeaderList* dictionary,
                const std::string& header
        );

        const std::map<std::string, std::string>&   get_header_map() const;

    private:
        HttpHeader(const HttpHeader& http_header);
        HttpHeader& operator=(const HttpHeader& http_header);

        std::map<std::string, std::string>  header_map_;
};

#endif