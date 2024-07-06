#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include "EventHandler.hpp"
# include "StreamHandler.hpp"
# include "Cgi.hpp"

# include <netdb.h>
# include <string>
# include <sys/socket.h>

class CgiHandler : public EventHandler {
    public:
        static struct addrinfo* ConvertToAddrInfo(const std::string& url);

        CgiHandler(StreamHandler& stream_handler, const struct addrinfo& address);

        virtual ~CgiHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        void    ReturnToStreamHandler();

        StreamHandler&  stream_handler_;
        Cgi          cgi_;
};

#endif  // PROXY_HANDLER_HPP_
