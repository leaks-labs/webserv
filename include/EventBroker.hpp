#ifndef EVENT_BROKER_HPP_
# define EVENT_BROKER_HPP_

# include <vector>

# ifdef __APPLE__
#  include <sys/event.h>
# elif __linux__
#  include <sys/epoll.h>
# endif

# include "ListenerList.hpp"

// TODO: to remove
# include <map>
#include "Response.hpp"

//

class EventBroker {
    public:
        EventBroker(const ListenerList& listeners);

        ~EventBroker();

        void    Run();

    private:
# ifdef __APPLE__
        typedef struct kevent       Event;
# elif __linux__
        typedef struct epoll_event  Event;
# endif

        static const int    kMaxEvents = 32;
        static const size_t kBufSize = 1024;

        EventBroker();
        EventBroker(const EventBroker& src);
        EventBroker&    operator=(const EventBroker& rhs);

# ifdef __APPLE__
        int     AddReadFilterFromListeners() const;
# endif
        int     AddReadFilter(int fd) const;
        int     AddWriteFilter(int fd) const;
        int     DelWriteFilter(int fd) const;
        int     GetIdent(const Event& event) const;
        bool    IsEventEOF(const Event& event) const;
        bool    IsEventRead(const Event& event) const;
        bool    IsEventWrite(const Event& event) const;
        int     WaitForEvents(std::vector<Event>& event_list, int event_list_size) const;
        void    WaitingLoop();
        void    HandleEvents(const std::vector<Event>& event_list, int number_events);
        bool    IsListener(int ident) const;
        void    AcceptConnection(int ident);
        void    DeleteConnection(int ident);
        void    SendData(const Event& event);
        void    ReceiveData(const Event& event);

        const ListenerList& listeners_;
        const int           queue_;
        // TODO: to replace with the future ClientList
        std::map<int, int> accepted_sfd_list_;
};

#endif  // EVENT_BROKER_HPP_

