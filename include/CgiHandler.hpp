#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include "ProcessHandler.hpp"
# include "HttpResponse.hpp"

class CgiHandler : public ProcessHandler {
    public:
        CgiHandler(StreamHandler& stream_handler, HttpResponse &response);
        virtual ~CgiHandler();
        virtual void    HandleEvent(EventTypes::Type event_type);

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        int  InitPipe();
        void CloseFd(int fd);
        void Fork();
        void Exec();

        Stream       stream_;
        int          pfd_[2];
};

#endif  // CGI_HANDLER_HPP_
