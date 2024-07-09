#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include "StreamHandler.hpp"
# include <netdb.h>
# include <string>
# include <sys/socket.h>

class CgiHandler : public EventHandler {
    public:

        CgiHandler(StreamHandler& stream_handler, std::string const & request);

        virtual ~CgiHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        int  InitPipe();
        void Fork();
        void Exec();
        void    ReturnToStreamHandler();

        StreamHandler &  stream_handler_;
        Stream       stream_;
        int          pfd_[2];
        std::string  request_; // this will be the request object
        std::string  response_; // this will be the response object
};

#endif  // PROXY_HANDLER_HPP_
