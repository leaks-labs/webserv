#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include "InitiationDispatcher.hpp"
# include "EventHandler.hpp"
# include "Stream.hpp"
# include "HttpMessage.hpp"
# include "HttpRequest.hpp"
# include <deque>
# include <fcntl.h>
# include <cerrno>
# include <cstring>
# include <stdexcept>
# include <string>
# include <unistd.h>

// TODO: to remove
#include <iostream>
// TODO: to remove

class HttpResponse;

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int acceptor_sfd, int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();
        
        void            Decode(std::string& buffer);
        void            Encode();
        
    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        int                         acceptor_sfd_;
        Stream                      stream_;
        std::deque<HttpResponse>    response_queue_;
};

#endif  // STREAM_HANDLER_HPP
