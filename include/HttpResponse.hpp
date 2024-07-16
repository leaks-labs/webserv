#ifndef HTTP_RESPONSE_HPP_
# define HTTP_RESPONSE_HPP_

# include "HttpRequest.hpp"
# include "HttpStatusLine.hpp"
# include "ServerList.hpp"

class StreamHandler;

class HttpResponse {
    public :
        HttpResponse(StreamHandler& stream_handler, HttpRequest& request);
        HttpResponse(const HttpResponse& src);

        ~HttpResponse();

        const std::string&              get_cgi_path() const;
        const std::string&              get_path() const;
        const std::string&              get_query() const;
        const std::vector<std::string>& get_env() const;
        std::string&                    get_request_body();
        std::string&                    get_response();

        void    set_status_line(int code);
        void    set_header(std::string& str);
        void    set_body(const std::string& str);

        void    Execute();
        bool    IsComplete() const;
        void    SetComplete();
        void    AddHeaderContentLength();
        bool    IsAskingToCloseConnection() const;
        void    AddErrorPageToBody(const int error);
        void    ClearHeader();
        void    UpdateReason();

    private:
        static std::string  FindExtension(const std::string& str);
        static bool         IsCgiFile(const std::string& path);
        static std::string  GetCgiPath(const std::string& ext);

        HttpResponse();
        HttpResponse&   operator=(const HttpResponse& rhs);

        std::string                 BuildPath();
        void                        Get();
        void                        Delete();
        void                        AddFileToBody();
        void                        AddListingPageToBody();
        void                        LaunchCgiHandler();
        void                        FinalizeResponse();
        std::vector<std::string>    SetEnv();

        bool                        complete_;
        HttpRequest&                request_;
        StreamHandler&              stream_handler_;
        HttpStatusLine              status_line_;
        bool                        keep_alive_;
        std::string                 path_;
        const std::string           cgi_path_;
        std::vector<std::string>    env_;
        HttpHeader                  header_;
        HttpBody                    body_;
        std::string                 response_;
};

#endif
