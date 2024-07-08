#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include "EventHandler.hpp"
# include "Stream.hpp"
# include "HttpMessage.hpp"

# include <deque>

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

        void            Decode(std::string& buffer);
        void            Execute(const HttpMessage& request);
        void            Encode(const HttpMessage& response);

    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        Stream                      stream_;
        std::deque<HttpMessage *>   requests_queue_;
        std::deque<HttpMessage *>   response_queue_;
        std::size_t                 request_count;
};

#endif  // STREAM_HANDLER_HPP
