#ifndef INITIATION_DISPATCHER_HPP_
# define INITIATION_DISPATCHER_HPP_

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
        static InitiationDispatcher&    Instance();

        virtual void    HandleEvents();
        int             RegisterHandler(EventHandler* event_handler, EventTypes::Type event_type);
        void            RemoveHandler(EventHandler* event_handler);
        int             AddReadFilter(EventHandler& event_handler);
        int             AddWriteFilter(EventHandler& event_handler);
        int             DelReadFilter(EventHandler& event_handler);
        int             DelWriteFilter(EventHandler& event_handler);
        void            Clear();

    private:
# ifdef __APPLE__
        typedef struct kevent       Event;
# elif __linux__
        typedef struct epoll_event  Event;
# endif

        static const int    kMaxEvents = 32;

        InitiationDispatcher();

        InitiationDispatcher(const InitiationDispatcher& src);
        InitiationDispatcher&   operator=(const InitiationDispatcher& rhs);

        ~InitiationDispatcher();

        int                     WaitForEvents(std::vector<Event>& event_list, int event_list_size) const;
        void                    IterateEventList(const std::vector<Event>& event_list, int nubmer_events);
        EventHandler::Handle    GetHandleFromEvent(const Event& event) const;
        EventTypes::Type        GetEventTypeFromEvent(const Event& event) const;
        bool                    IsEventRead(const Event& event) const;
        bool                    IsEventWrite(const Event& event) const;
        bool                    IsEventClose(const Event& event) const;

        std::map<EventHandler::Handle, EventHandler*>   event_handler_table_;
        const int                                       demultiplexer_;
};

#endif  // INITIATION_DISPATCHER_HPP_
