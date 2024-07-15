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
# include "Stream.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int acceptor_sfd, int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();
        
        int     ReRegister();
        int     UnRegister();
        void    AddToRequestQueue();
        int     SendFirstResponse();
        void    ConvertRequestToResponse();
        
    private:
        static const int    kKeepConnection = 0;
        static const int    kCloseConnection = 1;

        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        int                         acceptor_sfd_;
        Stream                      stream_;
        std::deque<HttpRequest>     request_queue_;
        std::deque<HttpResponse>    response_queue_;
};

#endif  // STREAM_HANDLER_HPP
