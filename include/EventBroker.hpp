#ifndef EVENT_BROKER_HPP_
# define EVENT_BROKER_HPP_

# include <vector>

# ifdef __APPLE__
#  include <sys/event.h>
# elif __linux__
#  include <sys/epoll.h>
# endif

# include "Listener.hpp"

class EventBroker {
    public:
        EventBroker(const std::vector<Listener*>& listeners);

        ~EventBroker();

        int run();

    private:
        static const int    kMaxEvents = 32;

        EventBroker();
        EventBroker(const EventBroker& src);
        EventBroker&    operator=(const EventBroker& rhs);

        bool    IsListener(int ident) const;
        int     AcceptConnection(int ident);
        void    DeleteConnection(int ident);
# ifdef __APPLE__
        void    SendData(struct kevent* event, char* buf /* replace with future request and response queue */);
        void    ReceiveData(struct kevent* event, char* buf /* replace with future request and response queue */);
# elif __linux__
        void    SendData(struct epoll_event* event, char* buf /* replace with future request and response queue */);
        void    ReceiveData(struct epoll_event* event, char* buf /* replace with future request and response queue */);
# endif

        int                             queue_;
        const std::vector<Listener*>&   listeners_;
        std::vector<int>                accepted_sfd_list_;
};

#endif  // EVENT_BROKER_HPP_