#include "EventBroker.hpp"

#include <algorithm>
#include <csignal>
#include <stdexcept>

#include <unistd.h>

#ifdef __APPLE__
# include <sys/event.h>
#elif __linux__
# include <sys/epoll.h>
#endif

// to remove
#include <iostream>
#include <cstdio>
// to remove


namespace 
{

volatile sig_atomic_t   g_signal_received = 0;

void    signal_handler(int signal)
{
    std::cout << std::endl;
    std::cout << "Signal " << signal << " received" << std::endl;
    g_signal_received = signal;
}

}

#ifdef __APPLE__

EventBroker::EventBroker(const std::vector<Listener*>& listeners)
    : listeners_(listeners)
{
    if ((queue_ = kqueue()) == -1)
        throw std::runtime_error("kqueue() failed");
        // TODO: or perror ?

    std::vector<struct kevent> events(listeners_.size());
    // TODO: check if events is empty or if vector throws exception

    // Populate the events array
    for (size_t i = 0; i < listeners_.size(); ++i)
        EV_SET(&events[i], listeners_[i]->get_sfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);

    // Register all events with a single kevent call
    int ret = kevent(queue_, events.data(), events.size(), NULL, 0, NULL);
    // TODO: what happens if kevent fails and set failed flag?
    if (ret == -1 /*|| static_cast<size_t>(ret) != events.size()*/) {
        // TODO: or perror ?
        close(queue_);
        throw std::runtime_error("kevent() registration failed");
    }
}

#elif __linux__

EventBroker::EventBroker(const std::vector<Listener*>& listeners)
    : listeners_(listeners)
{
    if ((queue_ = epoll_create(1)) == -1)
        throw std::runtime_error("epoll_create() failed");
        // TODO: or perror ?

    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it) {
        struct epoll_event  event = {};
        event.events = EPOLLIN;
        event.data.fd = (*it)->get_sfd();
        if (epoll_ctl(queue_, EPOLL_CTL_ADD, event.data.fd, &event) == -1) {
            // TODO: or perror ?
            for (std::vector<Listener*>::const_iterator it2 = listeners_.begin(); it2 != it; ++it2)
                epoll_ctl(queue_, EPOLL_CTL_DEL, (*it2)->get_sfd(), NULL);
                // TODO: what happens if kevent fails ?
            close(queue_);
            throw std::runtime_error("epoll_ctl() failed");
        }
    }
}

#endif

#ifdef __APPLE__

EventBroker::~EventBroker()
{
    size_t  total_size = listeners_.size() + accepted_sfd_list_.size();
    std::vector<struct kevent> events(total_size);
    // TODO: check if events is empty or if vector throws exception

    // Populate the events array
    std::vector<struct kevent>::iterator    it_events = events.begin();
    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it, ++it_events)
        EV_SET(&(*it_events), (*it)->get_sfd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    for (std::vector<int>::const_iterator it = accepted_sfd_list_.begin(); it != accepted_sfd_list_.end(); ++it, ++it_events)
        EV_SET(&(*it_events), *it, EVFILT_READ, EV_DELETE, 0, 0, NULL);

    // Register all events with a single kevent call
    kevent(queue_, events.data(), events.size(), NULL, 0, NULL);
    // TODO: what happens if kevent fails and set failed flag?

    std::for_each(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), close);

    close(queue_);
}

#elif __linux__

EventBroker::~EventBroker()
{
    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it)
        epoll_ctl(queue_, EPOLL_CTL_DEL, (*it)->get_sfd(), NULL);
        // TODO: what happens if kevent fails ?
    for (std::vector<int>::const_iterator it = accepted_sfd_list_.begin(); it != accepted_sfd_list_.end(); ++it) {
        epoll_ctl(queue_, EPOLL_CTL_DEL, *it, NULL);
        // TODO: what happens if kevent fails ?
        close(*it);
    }

    close(queue_);
}

#endif

#ifdef __APPLE__

int EventBroker::run()
{
    struct kevent   event[kMaxEvents];
    int             number_events;

    signal(SIGINT, signal_handler);

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        number_events = kevent(queue_, NULL, 0, event, kMaxEvents, NULL /* TODO: timeout ? */);
        // TODO: what happens if kevent fails?

        for (int i = 0; i < number_events && g_signal_received == 0; ++i) {
            if (IsListener(event[i].ident)) {
                std::cout << "ENTER: AcceptConnection " << std::endl;
                AcceptConnection(event[i].ident);
                // TODO: check error here?
            } else if (event[i].flags & EV_EOF) {
                std::cout << "ENTER: DeleteConnection " << std::endl;
                DeleteConnection(event[i].ident);
                // TODO: check error here?
            } else if (event[i].flags & EVFILT_READ) {
                std::cout << "ENTER: read and sending message " << std::endl;
                // TODO:
                // prepare the request
                // analyse request and send to state machine
                // prepare response
                // send response

                // TODO: for now, just echo back the request
                std::cout << "Received data from socket " << event[i].ident << std::endl;
                char buf[10000];
                int bytes_read = recv(event[i].ident, buf, sizeof(buf) - 1, 0 /* MSG_WAITALL */);
                buf[bytes_read] = 0;
                const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
                send(event[i].ident, response.c_str(), response.size(), 0);
            }

            // TODO: else ? ERROR flag ??
        }
    }
    return 0;
}

#elif __linux__

int EventBroker::run()
{
    struct epoll_event  event[kMaxEvents];
    int                 number_events;

    signal(SIGINT, signal_handler);

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        number_events = epoll_wait(queue_, event, kMaxEvents, -1 /* TODO: timeout ? */);
        // TODO: what happens if epoll fails?

        for (int i = 0; i < number_events && g_signal_received == 0; ++i) {
            if (IsListener(event[i].data.fd)) {
                std::cout << "ENTER: AcceptConnection " << std::endl;
                AcceptConnection(event[i].data.fd);
                // TODO: check error here?
            } else if ((event[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0u) {
                std::cout << "ENTER: DeleteConnection " << std::endl;
                DeleteConnection(event[i].data.fd);
                // TODO: check error here?
            } else if ((event[i].events & EPOLLIN) != 0u) {
                std::cout << "ENTER: read and sending message " << std::endl;
                // TODO:
                // prepare the request
                // analyse request and send to state machine
                // prepare response
                // send response

                // TODO: for now, just echo back the request
                std::cout << "Received data from socket " << event[i].data.fd << std::endl;
                char buf[10000];
                int bytes_read = recv(event[i].data.fd, buf, sizeof(buf) - 1, 0 /* MSG_WAITALL */);
                buf[bytes_read] = 0;
                const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
                send(event[i].data.fd, response.c_str(), response.size(), 0);
            }

            // TODO: else ? ERROR flag ??
        }
    }
    return 0;
}

#endif

bool    EventBroker::IsListener(int ident) const
{
    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it)
        if ((*it)->get_sfd() == ident)
            return (true);
    return (false);
}

#ifdef __APPLE__

int EventBroker::AcceptConnection(int ident)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    int new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (new_sfd == -1) {
        perror("accept() failed"); // signal error and continue
        return -1;
    }

    struct kevent   filter;
    EV_SET(&filter, new_sfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // TODO: what happens if kevent fails and set failed flag?
    if (kevent(queue_, &filter, 1, NULL, 0, NULL) == -1) {
        perror("kevent() failed"); // signal error and continue
        close(new_sfd);
        return -1;
    }

    accepted_sfd_list_.push_back(new_sfd);
    // TODO: check if push_back fails, and may be closes the socket, and delete from queue

    return 0;
}

#elif __linux__

int EventBroker::AcceptConnection(int ident)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    int new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (new_sfd == -1) {
        perror("accept() failed"); // signal error and continue
        return -1;
    }

    struct epoll_event   filter = {};
    filter.events = (EPOLLIN | EPOLLRDHUP);
    filter.data.fd = new_sfd;

    if (epoll_ctl(queue_, EPOLL_CTL_ADD, new_sfd, &filter) == -1) {
        perror("epoll_ctl() failed"); // signal error and continue
        close(new_sfd);
        return -1;
    }

    accepted_sfd_list_.push_back(new_sfd);
    // TODO: check if push_back fails, and may be closes the socket, and delete from queue

    return 0;
}

#endif

#ifdef __APPLE__

int EventBroker::DeleteConnection(int ident)
{
    struct kevent   filter;
    EV_SET(&filter, ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(queue_, &filter, 1, NULL, 0, NULL);
    // TODO: what happens if kevent fails and set failed flag?

    close(ident);
    accepted_sfd_list_.erase(std::find(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), ident));
    // TODO: is there a possibility that find return .last() and erase fails?

    return 0;
}

#elif __linux__

int EventBroker::DeleteConnection(int ident)
{
    epoll_ctl(queue_, EPOLL_CTL_DEL, ident, NULL);
    // TODO: what happens if kevent fails ?

    close(ident);
    accepted_sfd_list_.erase(std::find(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), ident));
    // TODO: is there a possibility that find return .last() and erase fails?

    return 0;
}

#endif
