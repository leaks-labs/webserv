#ifndef ACCEPTOR_HPP_
# define ACCEPTOR_HPP_

# include <netdb.h>

class Acceptor {
    public:
        Acceptor(const struct addrinfo& address);

        ~Acceptor();

        int get_sfd() const;

        int Accept() const;

    private:
        Acceptor();
        Acceptor(const Acceptor& src);
        Acceptor&   operator=(const Acceptor& rhs);

        const int   sfd_;
};

#endif  // ACCEPTOR_HPP_
