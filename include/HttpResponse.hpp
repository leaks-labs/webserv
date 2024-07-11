#ifndef HTTP_RESPONSE_HPP_
# define HTTP_RESPONSE_HPP_

# include "HttpMessage.hpp"
# include "HttpRequest.hpp"
# include "ServerList.hpp"
# include "Directory.hpp"
# include "PathFinder.hpp"

class HttpResponse
{
    public :
        HttpResponse(HttpRequest & request, int client_sfd);
        HttpResponse(HttpResponse const & src);
        //StreamHandler &get_stream_handler() const;
        HttpRequest &get_request() const;
        Server      const &get_server() const;
        Location    const *get_location() const;
        std::string const & get_request_path() const;
        std::string const & get_path() const;
        int                get_error() const;
        std::string  const & get_body() const;
        std::string  const & get_header() const;
        std::string         get_content() const;

        ~HttpResponse();

    private:
        HttpResponse();
        HttpResponse& operator=(HttpResponse const & src);
        std::string BuildPath();
        std::string FindExtension(std::string const & str) const;
        bool        IsCgiFile(std::string const & path) const;
        std::string GetCgiPath(std::string const & ext) const;
        std::string ReadFile();
        std::string CreateHeader();
        std::string CreateBody();
        std::string ReadDirectory(Directory & dir);

        //StreamHandler&  stream_handler_;
        HttpRequest &   request_;
        Server const &  server_;
        std::string     request_path_;
        Location const* location_;
        std::string     buffer_;
        std::string     path_;
        std::string     body_;
        std::string     header_;
        int             error_;
};

#endif