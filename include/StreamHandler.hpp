#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include "EventHandler.hpp"
# include "Stream.hpp"

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);

    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);

        Stream  stream_;
};

#endif  // STREAM_HANDLER_HPP
