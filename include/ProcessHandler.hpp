#ifndef PROCESS_HANDLER_HPP_
# define PROCESS_HANDLER_HPP_

#include "StreamHandler.hpp"

class ProcessHandler : public EventHandler {
    public:
        virtual ~ProcessHandler();
        virtual Handle  get_handle() const;
        virtual void    HandleEvent(EventTypes::Type event_type) = 0;
        virtual void    HandleTimeout();

    protected:
        ProcessHandler(StreamHandler& stream_handler, std::string &buffer);
        void            InitHandler();
        void            ReturnToStreamHandler();
        StreamHandler&  stream_handler_;
        std::string     buffer_;
        Stream          stream_;

    private:
        ProcessHandler();
        ProcessHandler(const ProcessHandler &src);
        ProcessHandler&   operator=(const ProcessHandler &rhs);

};

#endif  // PROXY_HANDLER_HPP_
