#ifndef LISTENER_HPP_
# define LISTENER_HPP_

# include <netdb.h>

class Listener {
    public:
        Listener(const struct addrinfo& address);

        ~Listener();

        int get_sfd() const;

    private:
        Listener();
        Listener(const Listener& src);
        Listener&   operator=(const Listener& rhs);

        int sfd_; /* socket file descriptor */
};

#endif  // LISTENER_HPP_