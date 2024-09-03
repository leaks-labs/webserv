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
        std::string&                    get_request_body_buffer();
        std::string&                    get_response_buffer();
        const HttpHeader&               get_header()const;

        void    set_status_line(int code);
        void    set_body(const std::string& str);
        void    set_request_host(const std::string& host);

        void        Execute();
        std::string GetCompleteRequet() const;
        void        ParseHeader(std::string& str);
        bool        HeaderIsComplete() const;
        void        ClearHeader();
        bool        IsComplete() const;
        void        SetComplete();
        void        AddHeaderContentLength();
        bool        IsAskingToCloseConnection() const;
        void        RedirectToNewTarget(int code);
        void        FinalizeResponse();
        void        Clear();

    private:
        static const int    kMaxRedirectCount = 10;

        static std::string  FindExtension(const std::string& str);
        static bool         IsCgiFile(const std::string& path);
        static std::string  GetCgiPath(const std::string& ext);
        static bool         IsDir(const std::string& path);
        static bool         HasRightToModify(const std::string& path);

        HttpResponse();
        HttpResponse&   operator=(const HttpResponse& rhs);

        void                        Apply();
        void                        ApplyGeneratedPage();
        void                        LaunchCgiHandler();
        bool                        IsHandledExternaly();
        void                        DeleteResource();
        void                        MovedPermanentely(const std::string& new_target);
        void                        AddHeaderLocation();
        void                        AddHeaderCloseConnection();
        void                        RedirectToEmptyTarget(int code);
        std::string                 BuildPath();
        void                        UpdatePathAndTarget(const std::string& new_target);
        void                        UpdateReason();
        std::string                 ErrorFileIsSet() const;
        std::vector<std::string>    SetEnv();

        bool                        complete_;
        HttpRequest&                request_;
        StreamHandler&              stream_handler_;
        const Server*               server_;
        const Location*             location_;
        HttpStatusLine              status_line_;
        bool                        keep_alive_;
        HttpRequestLine::Target&    target_;
        std::string                 path_;
        std::string                 cgi_path_;
        std::vector<std::string>    env_;
        HttpHeader                  header_;
        HttpBody                    body_;
        std::string                 response_;
        int                         redirect_count_;
        std::string                 moved_permanentely_path_;
};

#endif
