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

        static int  Split(const std::string& str, const std::string& delim, std::vector<std::string>& tokens);

        HttpRequest(int acceptor_fd);
        HttpRequest(const HttpRequest& src);
        HttpRequest&    operator=(const HttpRequest& rhs);

        ~HttpRequest();

        int                     get_status_code() const;
        const Server&           get_server() const;
        const Location&         get_location() const;
        const HttpRequestLine&  get_request_line() const;
        HttpRequestLine&        get_request_line();
        const HttpHeader&       get_header() const;
        const HttpBody&         get_body() const;
        std::string&            get_body();
        std::string             get_host() const;

        void        AppendToRequest(std::string& message);
        bool        IsComplete() const;
        std::string GetCompleteRequest() const;
        bool        KeepAlive() const;

    private:
        static std::map<int, std::string> InitStatusMap();

        HttpRequest();

        int                 acceptor_fd_;
        bool                is_complete_;
        int                 status_code_;
        const Server*       server_;
        const Location*     location_;
        HttpRequestLine     request_line_;
        HttpHeader          header_;
        HttpBody            body_;
};

#endif
