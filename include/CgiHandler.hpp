#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include "StreamHandler.hpp"
# include "HttpResponse.hpp"

class CgiHandler : public EventHandler {
    public:
        CgiHandler(StreamHandler& stream_handler, HttpResponse &response);
        virtual ~CgiHandler();
        virtual Handle  get_handle() const;
        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    ReturnToStreamHandler();
        virtual void    HandleTimeout();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        int  InitPipe();
        void CloseFd(int fd);
        void Fork();
        void Exec();

        int          pfd_[2];
        StreamHandler&  stream_handler_;
        HttpResponse&    response_;
        Stream          stream_;
};

#endif  // CGI_HANDLER_HPP_
