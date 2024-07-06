#ifndef REQUEST_HEADER_HPP_
# define REQUEST_HEADER_HPP_

# include <string>
# include <map>

class RequestHeader
{
    public:
        /* Constructor for RequestHeader class */
        RequestHeader();

        /* Destructor for RequestHeader class */
        ~RequestHeader();

        /* Copy constructor for RequestHeader class */
        RequestHeader(const RequestHeader& request_header);

        /* Assignment constructor for RequestHeader class */
        RequestHeader& operator=(const RequestHeader& request_header);

        /* is valid
             verify header validity by type
                Accept-Charset
                Accept-Encoding
                Accept-Language
                Authorization
                Expect
                From
                Host
                If-Match
                If-Modified-Since
                If-None-Match
                If-Range
                If-Unmodified-Since
                Max-Forwards
                Proxy-Authorization
                Range
                Referer
                TE
                User-Agent
         add to map
            check if already exist
                if yes verify if rule already exist
                    if no just add comma and join
                    if rule already exist throw an error
                if no just add it
        get from map */
    private:
        std::map<std::string, std::string> header_map_;
        /* bool is user listening ? */
};

#endif