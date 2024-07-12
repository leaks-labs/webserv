#ifndef CGI_HANDLER_HPP_
# define CGI_HANDLER_HPP_

# include <utility>

# include "EventHandler.hpp"
# include "EventTypes.hpp"
# include "StreamHandler.hpp"
# include "HttpResponse.hpp"

class CgiHandler : public EventHandler {
    public:
        CgiHandler(StreamHandler& stream_handler, HttpResponse& response);

        virtual ~CgiHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler &src);
        CgiHandler&   operator=(const CgiHandler &rhs);

        std::pair<int, int> InitSocketPair();
        void                ExecCGI();
        void                KillChild();
        void                ReturnToStreamHandler();

        StreamHandler&      stream_handler_;
        HttpResponse&       response_;
        std::pair<int, int> sfd_pair_;
        Stream              stream_main_;
        Stream              stream_child_;
        pid_t               pid_child_;
};

#endif  // CGI_HANDLER_HPP_
