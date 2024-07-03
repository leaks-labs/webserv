#ifndef HTTP_HEADER_
# define HTTP_HEADER_

# include <map>
# include <vector>
# include <string>

# define kDefaultDictionary "header_dictionary.json"

class HttpHeader
{
    public:
        HttpHeader(const std::string& dictionary);
        ~HttpHeader();

        const std::map<std::string, std::vector<std::string> >&
        BuildDictionary(const std::string& dictionary);

        const std::map<std::string, std::string>& get_header_map() const;

    private:
        HttpHeader();
        HttpHeader(const HttpHeader& http_header);
        HttpHeader& operator=(const HttpHeader& http_header);

        const std::map<std::string, std::vector<std::string> >  header_dictionary_;
        std::map<std::string, std::string>                      header_map_;
};

#endif