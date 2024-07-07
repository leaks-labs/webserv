#include "InitiationDispatcher.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

#include <unistd.h>

// to remove
#include <iostream>
// to remove

volatile sig_atomic_t   InitiationDispatcher::g_signal_received = 0;

InitiationDispatcher&   InitiationDispatcher::Instance()
{
    static InitiationDispatcher instance;
    return instance;
}

void    InitiationDispatcher::HandleEvents(const time_t timeout)
{
    if (signal(SIGINT, SignalHandler) == SIG_ERR)
        throw std::runtime_error("signal() failed: " + std::string(strerror(errno)));
    time_t  start_time;
    if (timeout >= 0 && (start_time = std::time(NULL)) == -1)
        throw std::runtime_error("time() failed: ");
    std::vector<Event>  event_list(kMaxEvents);
    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        int number_events = WaitForEvents(event_list, kMaxEvents, timeout);
        if (number_events == -1) {
            if (errno != EINTR)
                perror("ERROR: WaitForEvents() failed to wait for events");
            continue;
            // TODO: is it a good idea to continue? Because we may have an infinite loop
        }
        IterateEventList(event_list, number_events);
        if (timeout >= 0)
            CheckForTimeouts(start_time, timeout);
    }
}

int InitiationDispatcher::RegisterHandler(EventHandler* event_handler, EventTypes::Type event_type)
{
    try
    {
        if ((EventTypes::IsReadEvent(event_type) && AddReadFilter(*event_handler) == -1)
            || (EventTypes::IsWriteEvent(event_type) && AddWriteFilter(*event_handler) == -1))
            return -1;
        event_handler_table_[event_handler->get_handle()] = event_handler;
        return 0;
    }
    catch(const std::exception& e)
    {
        return -1;
    }
}

void    InitiationDispatcher::RemoveHandler(EventHandler* event_handler)
{
    event_handler_table_.erase(event_handler->get_handle());
    delete event_handler;
}

int InitiationDispatcher::DeactivateHandler(EventHandler& event_handler)
{
#ifdef __APPLE__
    return (DelReadFilter(event_handler) == -1 || DelWriteFilter(event_handler) == -1) ? -1 : 0;
#elif __linux__
    return (epoll_ctl(demultiplexer_, EPOLL_CTL_DEL, event_handler.get_handle(), NULL));
#endif
}

int InitiationDispatcher::AddReadFilter(EventHandler& event_handler)
{
    if (EventTypes::IsReadEvent(event_handler.get_event_types_registred()))
        return 0;
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, event_handler.get_handle(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(demultiplexer_, &event, 1, NULL, 0, NULL) == 0) {
        event_handler.AddRegistredEvent(EventTypes::kReadEvent);
        return 0;
    }
#elif __linux__
    event.events = (EPOLLIN | EPOLLRDHUP);
    if (EventTypes::IsWriteEvent(event_handler.get_event_types_registred()))
        event.events |= EPOLLOUT;
    event.data.fd = event_handler.get_handle();
    if (epoll_ctl(demultiplexer_, EPOLL_CTL_ADD, event.data.fd, &event) == 0
        || (errno == EEXIST && epoll_ctl(demultiplexer_, EPOLL_CTL_MOD, event.data.fd, &event) == 0)) {
        event_handler.AddRegistredEvent(EventTypes::kReadEvent);
        return 0;
    }
#endif
    return -1;
}

int InitiationDispatcher::AddWriteFilter(EventHandler& event_handler)
{
    if (EventTypes::IsWriteEvent(event_handler.get_event_types_registred()))
        return 0;
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, event_handler.get_handle(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(demultiplexer_, &event, 1, NULL, 0, NULL) == 0) {
        event_handler.AddRegistredEvent(EventTypes::kWriteEvent);
        return 0;
    }
#elif __linux__
    event.events = (EPOLLOUT | EPOLLRDHUP);
    if (EventTypes::IsReadEvent(event_handler.get_event_types_registred()))
        event.events |= EPOLLIN;
    event.data.fd = event_handler.get_handle();
    if (epoll_ctl(demultiplexer_, EPOLL_CTL_ADD, event.data.fd, &event) == 0
        || (errno == EEXIST && epoll_ctl(demultiplexer_, EPOLL_CTL_MOD, event.data.fd, &event) == 0)) {
        event_handler.AddRegistredEvent(EventTypes::kWriteEvent);
        return 0;
    }
#endif
    return -1;
}

int InitiationDispatcher::DelReadFilter(EventHandler& event_handler)
{
    if (!EventTypes::IsReadEvent(event_handler.get_event_types_registred()))
        return 0;
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, event_handler.get_handle(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(demultiplexer_, &event, 1, NULL, 0, NULL) == 0) {
        event_handler.DelRegistredEvent(EventTypes::kReadEvent);
        return 0;
    }
#elif __linux__
    event.events = EPOLLRDHUP;
    if (EventTypes::IsWriteEvent(event_handler.get_event_types_registred()))
        event.events |= EPOLLOUT;
    event.data.fd = event_handler.get_handle();
    if (epoll_ctl(demultiplexer_, EPOLL_CTL_MOD, event.data.fd, &event) == 0) {
        event_handler.DelRegistredEvent(EventTypes::kReadEvent);
        return 0;
    }
#endif
    return -1;
}

int InitiationDispatcher::DelWriteFilter(EventHandler& event_handler)
{
    if (!EventTypes::IsWriteEvent(event_handler.get_event_types_registred()))
        return 0;
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, event_handler.get_handle(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(demultiplexer_, &event, 1, NULL, 0, NULL) == 0) {
        event_handler.DelRegistredEvent(EventTypes::kWriteEvent);
        return 0;
    }
#elif __linux__
    event.events = EPOLLRDHUP;
    if (EventTypes::IsReadEvent(event_handler.get_event_types_registred()))
        event.events |= EPOLLIN;
    event.data.fd = event_handler.get_handle();
    if (epoll_ctl(demultiplexer_, EPOLL_CTL_MOD, event.data.fd, &event) == 0) {
        event_handler.DelRegistredEvent(EventTypes::kWriteEvent);
        return 0;
    }
#endif
    return -1;
}

void    InitiationDispatcher::Clear()
{
    for (std::map<EventHandler::Handle, EventHandler*>::const_iterator it = event_handler_table_.begin(); it != event_handler_table_.end(); ++it)
        delete it->second;
    event_handler_table_.clear();
}

void    InitiationDispatcher::SignalHandler(int signal)
{
    std::cout << std::endl;
    std::cout << "Signal " << signal << " received" << std::endl;
    g_signal_received = signal;
}

EventHandler::Handle    InitiationDispatcher::GetHandleFromEvent(const Event& event)
{
#ifdef __APPLE__
    return event.ident;
#elif __linux__
    return event.data.fd;
#endif
}

EventTypes::Type   InitiationDispatcher::GetEventTypeFromEvent(const Event& event)
{
    EventTypes::Type   event_type = EventTypes::kNoEvent;
    if (IsEventClose(event))
        event_type |= EventTypes::kCloseEvent;
    if (IsEventRead(event))
        event_type |= EventTypes::kReadEvent;
    if (IsEventWrite(event))
        event_type |= EventTypes::kWriteEvent;
    return event_type;
}

bool    InitiationDispatcher::IsEventRead(const Event& event)
{
#ifdef __APPLE__
    return (event.filter == EVFILT_READ);
#elif __linux__
    return ((event.events & EPOLLIN) != 0);
#endif
}

bool    InitiationDispatcher::IsEventWrite(const Event& event)
{
#ifdef __APPLE__
    return (event.filter == EVFILT_WRITE);
#elif __linux__
    return ((event.events & EPOLLOUT) != 0);
#endif
}

bool    InitiationDispatcher::IsEventClose(const Event& event)
{
#ifdef __APPLE__
    return (event.flags & EV_EOF);
#elif __linux__
    return ((event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0);
#endif
}

InitiationDispatcher::InitiationDispatcher()
#ifdef __APPLE__
    : demultiplexer_(kqueue())
#elif __linux__
    : demultiplexer_(epoll_create(1))
#endif
{
    if (demultiplexer_ == -1)
        throw std::runtime_error("failed to create the demultiplexer: " + std::string(std::strerror(errno)));
}

InitiationDispatcher::~InitiationDispatcher()
{
    Clear();
    close(demultiplexer_);
}

int InitiationDispatcher::WaitForEvents(std::vector<Event>& event_list, int event_list_size, const time_t timeout) const
{
#ifdef __APPLE__
    const struct timespec*  timeout_timespec_ptr = NULL;
    struct timespec         timeout_timespec;
    if (timeout >= 0) {
        timeout_timespec.tv_sec = timeout;
        timeout_timespec.tv_nsec = 0;
        timeout_timespec_ptr = &timeout_timespec;
    }
    return kevent(demultiplexer_, NULL, 0, event_list.data(), event_list_size, timeout_timespec_ptr);
#elif __linux__
    int timeout_ms = -1;
    if (timeout >= 0) {
        timeout_ms = static_cast<int>(timeout * 1000);
        if (timeout_ms < -1)
            timeout_ms = -1;
    }
    return epoll_wait(demultiplexer_, event_list.data(), event_list_size, timeout_ms);
#endif
}

void    InitiationDispatcher::IterateEventList(const std::vector<Event>& event_list, int number_events)
{
    for (std::vector<Event>::const_iterator it = event_list.begin(); number_events > 0 && g_signal_received == 0; ++it, --number_events) {
        std::cout << "socket: " << GetHandleFromEvent(*it) << std::endl;
        try
        {
            std::map<EventHandler::Handle, EventHandler*>::iterator entry = event_handler_table_.find(GetHandleFromEvent(*it));
            if (entry != event_handler_table_.end())
                entry->second->HandleEvent(GetEventTypeFromEvent(*it));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void    InitiationDispatcher::CheckForTimeouts(time_t& start_time, time_t timeout)
{
    time_t  current_time = std::time(NULL);
    if (current_time == -1) {
        std::cerr << "time() failed: timeout handler not called" << std::endl;
    } else if (current_time - start_time >= timeout) {
        std::cout << "Callig timeout handler..." << std::endl;
        start_time = current_time;
        for (std::map<EventHandler::Handle, EventHandler*>::iterator it = event_handler_table_.begin(); it != event_handler_table_.end();) {
            std::map<EventHandler::Handle, EventHandler*>::iterator next = it;
            ++next;
            it->second->HandleTimeout();
            it = next;
        }
    }
}
