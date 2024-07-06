#ifndef CONNECTION_ACCEPTOR_HPP_
# define CONNECTION_ACCEPTOR_HPP_

# include "Acceptor.hpp"
# include "EventHandler.hpp"

class ConnectionAcceptor : public EventHandler {
    public:
        ConnectionAcceptor(const struct addrinfo& address);

        virtual ~ConnectionAcceptor();

        virtual Handle  get_handle() const;

        virtual void    HandleEvent(EventTypes::Type event_type);
        virtual void    HandleTimeout();

    private:
        ConnectionAcceptor(const ConnectionAcceptor& src);
        ConnectionAcceptor& operator=(const ConnectionAcceptor& rhs);

        const Acceptor  acceptor_;
};

#endif  // CONNECTION_ACCEPTOR_HPP_