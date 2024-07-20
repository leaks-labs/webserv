#ifndef INITIATION_DISPATCHER_HPP_
# define INITIATION_DISPATCHER_HPP_

# include <csignal>
# include <ctime>
# include <map>
# include <vector>

# include "EventHandler.hpp"
# include "EventTypes.hpp"

# ifdef __APPLE__
#  include <sys/event.h>
# elif __linux__
#  include <sys/epoll.h>
# endif

class InitiationDispatcher {
    public:
        typedef std::multimap<time_t, EventHandler*>::iterator  TimeoutIterator;

        static const time_t kNoTimeout = -1;
        // kTimeWatingEvent MUST be less than the other timeouts (time in seconds)
        static const time_t kTimeWaitingEvent = 5;
        static const time_t kRequestTimeout = 20;
        static const time_t kPluginTimeout = 20;

        static InitiationDispatcher&    Instance();

        virtual void    HandleEvents(const time_t timeout);
        int             RegisterHandler(EventHandler* event_handler, EventTypes::Type event_type);
        void            RemoveHandler(EventHandler* event_handler);
        void            RemoveEntry(EventHandler* event_handler);
        int             DeactivateHandler(EventHandler& event_handler);
        int             AddReadFilter(EventHandler& event_handler);
        int             AddWriteFilter(EventHandler& event_handler);
        int             DelReadFilter(EventHandler& event_handler);
        int             DelWriteFilter(EventHandler& event_handler);
        int             SwitchFromWriteToRead(EventHandler& event_handler);
        void            Clear();
        TimeoutIterator AddTimeout(EventHandler* event_handler, time_t timeout);
        TimeoutIterator DelTimeout(TimeoutIterator timeout_it);

    private:
# ifdef __APPLE__
        typedef struct kevent       Event;
# elif __linux__
        typedef struct epoll_event  Event;
# endif

        static const int                kMaxEvents = 32;

        static volatile sig_atomic_t    g_signal_received;

        static void                 SignalHandler(int signal);
        static EventHandler::Handle GetHandleFromEvent(const Event& event);
        static EventTypes::Type     GetEventTypeFromEvent(const Event& event);
        static bool                 IsEventRead(const Event& event);
        static bool                 IsEventWrite(const Event& event);
        static bool                 IsEventCloseRead(const Event& event);
        static bool                 IsEventCloseWrite(const Event& event);
        static time_t               GetNow();

        InitiationDispatcher();

        InitiationDispatcher(const InitiationDispatcher& src);
        InitiationDispatcher&   operator=(const InitiationDispatcher& rhs);

        ~InitiationDispatcher();

        int     WaitForEvents(std::vector<Event>& event_list, int event_list_size, const time_t timeout) const;
        void    IterateEventList(const std::vector<Event>& event_list, int nubmer_events);
        void    CheckForTimeouts();

        std::map<EventHandler::Handle, EventHandler*>   event_handler_table_;
        const int                                       demultiplexer_;
        std::multimap<time_t, EventHandler*>            timeout_table_;
};

#endif  // INITIATION_DISPATCHER_HPP_
