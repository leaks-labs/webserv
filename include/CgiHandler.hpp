#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include "EventHandler.hpp"
# include "Cgi.hpp"
# include <netdb.h>
# include <string>
# include <sys/socket.h>

class CgiHandler : public EventHandler {
    public:

        CgiHandler(EventHandler& stream_handler, std::string const & request);

        virtual ~CgiHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        void    ReturnToStreamHandler();

        EventHandler &  stream_handler_;
        int          sfd_[2];
        Cgi          cgi_;
};

#endif  // PROXY_HANDLER_HPP_
