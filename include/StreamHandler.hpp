#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include <deque>
# include <string>

# include "EventHandler.hpp"
# include "EventTypes.hpp"
# include "Stream.hpp"

# include <fcntl.h>
# include <unistd.h>

// TODO: to remove
#include <iostream>
// TODO: to remove

class HttpRequest;
class HttpResponse;

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int acceptor_sfd, int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();
        
        int     ReRegister();
        int     UnRegister();
        void    Decode(std::string& buffer);
        void    Encode();
        
    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        int                         acceptor_sfd_;
        Stream                      stream_;
        std::deque<HttpRequest*>    request_queue_;
        std::deque<HttpResponse*>   response_queue_;
        std::string                 read_buffer;
};

#endif  // STREAM_HANDLER_HPP
