#ifndef PROXY_HANDLER_HPP_
# define PROXY_HANDLER_HPP_

# include "ProcessHandler.hpp"
# include "HttpResponse.hpp"

# include <netdb.h>
# include <string>
# include <sys/socket.h>

class ProxyHandler : public ProcessHandler {
    public:
        static struct addrinfo* ConvertToAddrInfo(const std::string& url);

        ProxyHandler(StreamHandler& stream_handler, HttpResponse& response, const struct addrinfo& address);

        virtual ~ProxyHandler();
        virtual void    HandleEvent(EventTypes::Type event_type);

    private:
        ProxyHandler();
        ProxyHandler(const ProxyHandler &src);
        ProxyHandler&   operator=(const ProxyHandler &rhs);
};

#endif  // PROXY_HANDLER_HPP_
