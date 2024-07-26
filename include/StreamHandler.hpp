#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include <deque>
# include <string>

# include <fcntl.h>
# include <unistd.h>

# include "EventHandler.hpp"
# include "EventTypes.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "InitiationDispatcher.hpp"
# include "Stream.hpp"

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int acceptor_sfd, int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();
        
        bool    AddToRequestQueue();
        bool    SendFirstResponse();
        void    ConvertRequestToResponse();
        
    private:
        static const bool   kKeepConnection = 0;
        static const bool   kCloseConnection = 1;

        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        int                                     acceptor_sfd_;
        Stream                                  stream_;
        std::deque<HttpRequest>                 request_queue_;
        std::deque<HttpResponse>                response_queue_;
        InitiationDispatcher::TimeoutIterator   timeout_it_;
        bool                                    should_close_connection_;
};

#endif  // STREAM_HANDLER_HPP
