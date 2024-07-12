#ifndef HTTP_RESPONSE_HPP_
# define HTTP_RESPONSE_HPP_

# include "HttpMessage.hpp"
# include "HttpRequest.hpp"
# include "ServerList.hpp"
# include "Directory.hpp"
# include "PathFinder.hpp"
# include "CgiHandler.hpp"

class HttpResponse
{
    public :
        HttpResponse(StreamHandler &stream_handler, HttpRequest *request, int client_sfd);
        HttpResponse(HttpResponse const & src);
        StreamHandler&          get_stream_handler() const;
        HttpRequest*            get_request() const;
        Server const &          get_server() const;
        Location const *        get_location() const;
        std::string const &     get_request_path() const;
        std::string const &     get_path() const;
        std::string const &     get_args() const;
        int                     get_error() const;
        std::string const &     get_body() const;
        std::string const &     get_header() const;
        std::string &           get_buffer();
        std::string const &     get_cgi_path() const;
        bool                    get_complete() const;

        void                    SetComplete();
        void                    AddToBody(std::string const & str);
        void                    CgiParseHeader();

        ~HttpResponse();

    private:
        HttpResponse();
        HttpResponse& operator=(HttpResponse const & src);
        std::string ExtractArgs();
        std::string BuildPath();
        std::string FindExtension(std::string const & str) const;
        bool        IsCgiFile(std::string const & path) const;
        std::string GetCgiPath(std::string const & ext) const;
        void        ReadFile();
        void        ReadError(const int error);
        void        CreateHeader();
        void        ReadDirectory(Directory & dir);
        void        LaunchCgiHandler();
        void        Get();

        int             error_;
        StreamHandler&  stream_handler_;
        HttpRequest*    request_;
        Server const &  server_;
        std::string     request_path_;
        std::string     args_;
        Location const* location_;
        std::string     buffer_;
        std::string     path_;
        std::string     header_;
        std::string     body_;
        std::string     cgi_path_;
        bool            complete_;
};

#endif