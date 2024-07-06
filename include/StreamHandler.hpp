#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include "EventHandler.hpp"
# include "Stream.hpp"

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int acceptor_sfd, int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        const int acceptor_sfd_;
        Stream      stream_;
        std::size_t request_count;
};

#endif  // STREAM_HANDLER_HPP
