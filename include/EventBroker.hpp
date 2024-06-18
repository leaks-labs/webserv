#ifndef EVENT_BROKER_HPP_
# define EVENT_BROKER_HPP_

# include <vector>

# ifdef __APPLE__
#  include <sys/event.h>
# elif __linux__
#  include <sys/epoll.h>
# endif

# include "ListenerListInfo.hpp"

class EventBroker {
    public:
        EventBroker(const ListenerList& listeners);

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
        void    SendData(const struct kevent& event, char* buf /* replace with future request and response queue */);
        void    ReceiveData(const struct kevent& event, char* buf /* replace with future request and response queue */);
# elif __linux__
        void    SendData(const struct epoll_event& event, char* buf /* replace with future request and response queue */);
        void    ReceiveData(const struct epoll_event& event, char* buf /* replace with future request and response queue */);
# endif

        const ListenerList& listeners_;
        const int           queue_;
        std::vector<int>    accepted_sfd_list_;
};

#endif  // EVENT_BROKER_HPP_
