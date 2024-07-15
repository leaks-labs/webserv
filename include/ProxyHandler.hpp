#ifndef PROXY_HANDLER_HPP_
# define PROXY_HANDLER_HPP_

# include "EventHandler.hpp"
# include "StreamHandler.hpp"
# include "Stream.hpp"

# include <netdb.h>
# include <string>
# include <sys/socket.h>

class ProxyHandler : public EventHandler {
    public:
        static struct addrinfo* ConvertToAddrInfo(const std::string& url);

        ProxyHandler(StreamHandler& stream_handler,  const struct addrinfo& address);

        virtual ~ProxyHandler();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        ProxyHandler();
        ProxyHandler(const ProxyHandler &src);
        ProxyHandler&   operator=(const ProxyHandler &rhs);

        void    ReturnToStreamHandler();

        StreamHandler&  stream_handler_;
        Stream          stream_;
};

#endif  // PROXY_HANDLER_HPP_
