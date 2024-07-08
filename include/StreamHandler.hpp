#ifndef STREAM_HANDLER_HPP
# define STREAM_HANDLER_HPP

# include "EventHandler.hpp"
# include "Stream.hpp"
# include "CgiHandler.hpp"
# include <deque>

class StreamHandler : public EventHandler {
    public:
        StreamHandler(int sfd);

        virtual ~StreamHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        StreamHandler();
        StreamHandler(const StreamHandler& src);
        StreamHandler&  operator=(const StreamHandler& rhs);
        void AddCgiHandler();

        Stream      stream_;
        std::size_t request_count;
        std::deque<CgiHandler> cgi_handlers_;
};

#endif  // STREAM_HANDLER_HPP
