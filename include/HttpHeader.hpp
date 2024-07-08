#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <vector>
# include <string>

# define N_HEADERS 11
# define N_HEADERS_ARGS 4

struct HeaderField
{
    std::string name;
    std::string args[N_HEADERS_ARGS];
    bool        is_used;
};

class HttpHeader
{
    public:
        HttpHeader();
        ~HttpHeader();

        static const HeaderField header_dictionary[N_HEADERS];

        static size_t                               FindHeader(
                const HeaderField* header_field,
                const std::string& header
        );

        const std::map<std::string, std::string>& get_header_map() const;

        void Parse(const std::string& header);

    private:
        HttpHeader(const HttpHeader& http_header);
        HttpHeader& operator=(const HttpHeader& http_header);

        std::map<std::string, std::string> header_map_;
};

#endif