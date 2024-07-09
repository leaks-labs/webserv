#ifndef LOXAL_HANDLER_HPP_
# define LOXAL_HANDLER_HPP_

# include "StreamHandler.hpp"

class LocalHandler : public EventHandler {
    public:

        LocalHandler(StreamHandler& stream_handler, std::string const & request);

        virtual ~LocalHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        LocalHandler();
        LocalHandler(const LocalHandler &src);
        LocalHandler&   operator=(const LocalHandler &rhs);

        int OpenFile();
        void    ReturnToStreamHandler();

        StreamHandler &  stream_handler_;
        Stream       stream_;
        std::string  request_; // this will be the request object
        std::string  response_; // this will be the response object
};

#endif  // PROXY_HANDLER_HPP_
