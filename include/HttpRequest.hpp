#ifndef HTTP_REQUEST_HPP_
# define HTTP_REQUEST_HPP_

# include <map>
# include <string>
# include <utility>
# include <vector>

# include "HttpRequestLine.hpp"
# include "HttpHeader.hpp"
# include "HttpBody.hpp"
# include "ServerList.hpp"

class HttpRequest {
    public :
        static const std::map<int, std::string> status_map;

        static void     Split(const std::string& str, const std::string& delim, std::vector<std::string>& tokens);
        static size_t   FindEndOfHeaders(const std::string& buff);

        HttpRequest(int acceptor_fd);
        HttpRequest(const HttpRequest& src);
        HttpRequest&    operator=(const HttpRequest& rhs);


        ~HttpRequest();

        const Server&           get_server() const;
        const Location&         get_location() const;
        const HttpRequestLine&  get_request_line() const;
        const HttpHeader&       get_header() const;
        const HttpBody&         get_body() const;
        int                     get_status() const;
        std::string             get_host() const;

        void    set_status(int status);
        void    set_is_complete(bool is_complete);
        void    set_is_parsed(bool is_parsed);

        void        AppendToRequest(std::string& message);
        bool        IsParsed() const;
        bool        IsComplete() const;
        std::string GetCompleteRequest() const;
        bool        KeepAlive() const;

    private:
        HttpRequest();
        static const int    kMaxRequestLineSize = 8192;
        static const int    kMaxRequestSize = 40960;
        static const int    kNotFound = 0;
        static std::map<int, std::string> InitStatusMap();

        void    Parse();
        void    UpdateBufferAndConsumeMessage(size_t request_length, std::string& message);
        void    CheckMaxRequestSize() const;

        int                 acceptor_fd_;
        HttpRequestLine     request_line_;
        HttpHeader          header_;
        HttpBody            body_;
        int                 status_code_;
        std::string         buffer_;
        bool                is_parsed_;
        bool                is_complete_;
        const Server*       server_;
        const Location*     location_;
};

#endif
