#include "EventBroker.hpp"

#include <algorithm>
#include <csignal>
#include <stdexcept>

#include <unistd.h>


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
        throw std::runtime_error("kqueue() failed to create the queue");

    std::vector<struct kevent>  events(listeners_.size());

    for (size_t i = 0; i < listeners_.size(); ++i)
        EV_SET(&events[i], listeners_[i]->get_sfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(queue_, events.data(), events.size(), NULL, 0, NULL) == -1) {
        close(queue_);
        throw std::runtime_error("kevent() failed to register listeners in the queue");
    }
}

#elif __linux__

EventBroker::EventBroker(const std::vector<Listener*>& listeners)
    : listeners_(listeners)
{
    if ((queue_ = epoll_create(1)) == -1)
        throw std::runtime_error("epoll_create() failed to create the queue");

    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it) {
        struct epoll_event  event = {};
        event.events = EPOLLIN;
        event.data.fd = (*it)->get_sfd();
        if (epoll_ctl(queue_, EPOLL_CTL_ADD, event.data.fd, &event) == -1) {
            close(queue_);
            throw std::runtime_error("epoll_ctl() failed to register listeners in the queue");
        }
    }
}

#endif

EventBroker::~EventBroker()
{
    std::for_each(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), close);
    close(queue_);
}

#ifdef __APPLE__

int EventBroker::run()
{
    struct kevent   event[kMaxEvents];
    int             number_events;

    // TODO: to remove, it's just to simulate request and response queue
    char buf[100000];
    *buf = '\0';
    // TODO: to remove, it's just to simulate request and response queue

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("ERROR: signal() failed");
        return -1;
    }

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        number_events = kevent(queue_, NULL, 0, event, kMaxEvents, NULL);
        // TODO: what happens if kevent fails?

        for (int i = 0; i < number_events && g_signal_received == 0; ++i) {
            std::cout << "socket: " << event[i].ident << std::endl;
            if (IsListener(event[i].ident)) {
                std::cout << "ENTER: AcceptConnection " << std::endl;
                AcceptConnection(event[i].ident);
                // TODO: check error here?
            } else if (event[i].flags & EV_EOF) {
                std::cout << "ENTER: DeleteConnection " << std::endl;
                DeleteConnection(event[i].ident);
                // TODO: check error here?

                // TODO: delete all pending requests and responses for that connection

            } else {
                if (event->filter == EVFILT_WRITE /* TODO: && a response is ready for that fd */ && *buf != '\0') {
                    std::cout << "ENTER: SendData " << std::endl;
                    SendData(&event[i], buf);
                    // TODO: check error here?
                }
                if (event->filter == EVFILT_READ) {
                    std::cout << "ENTER: ReceiveData " << std::endl;
                    ReceiveData(&event[i], buf);
                    // TODO: check error here?
                }
            }
        }

        // TODO: analyse all complete requests and create responses

    }

    // TODO: clean all pending requests and responses

    return 0;
}

#elif __linux__

int EventBroker::run()
{
    struct epoll_event  event[kMaxEvents];
    int                 number_events;

    // TODO: to remove, it's just to simulate request and response queue
    char buf[10000];
    *buf = '\0';
    // TODO: to remove, it's just to simulate request and response queue

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("ERROR: signal() failed");
        return -1;
    }

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        number_events = epoll_wait(queue_, event, kMaxEvents, -1);
        // TODO: what happens if epoll fails?

        for (int i = 0; i < number_events && g_signal_received == 0; ++i) {
            std::cout << "socket: " << event[i].data.fd << std::endl;
            if (IsListener(event[i].data.fd)) {
                std::cout << "ENTER: AcceptConnection " << std::endl;
                AcceptConnection(event[i].data.fd);
                // TODO: check error here?
            } else if (event[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                std::cout << "ENTER: DeleteConnection " << std::endl;
                DeleteConnection(event[i].data.fd);
                // TODO: check error here?

                // TODO: delete all pending requests and responses for that connection

            } else {
                if (event->events & EPOLLOUT /* TODO: && a response is ready for that fd */ && *buf != '\0') {
                    std::cout << "ENTER: SendData " << std::endl;
                    SendData(&event[i], buf);
                    // TODO: check error here?
                }
                if (event->events & EPOLLIN) {
                    std::cout << "ENTER: ReceiveData " << std::endl;
                    ReceiveData(&event[i], buf);
                    // TODO: check error here?
                }
            }
        }

        // TODO: analyse all complete requests and create responses

    }

    // TODO: clean all pending requests and responses

    return 0;
}

#endif

bool    EventBroker::IsListener(int ident) const
{
    for (std::vector<Listener*>::const_iterator it = listeners_.begin(); it != listeners_.end(); ++it)
        if ((*it)->get_sfd() == ident)
            return true;
    return false;
}

#ifdef __APPLE__

int EventBroker::AcceptConnection(int ident)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    int new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (new_sfd == -1) {
        perror("ERROR: accept() failed to accept a new client");
        return -1;
    }

    struct kevent   filter;
    EV_SET(&filter, new_sfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(queue_, &filter, 1, NULL, 0, NULL) == -1) {
        perror("ERROR: kevent() failed to register a new client in the queue");
        close(new_sfd);
        return -1;
    }

    try
    {
        accepted_sfd_list_.push_back(new_sfd);
        return 0;
    }
    catch(const std::exception& e)
    {
		std::cerr << "ERROR: " << e.what() << std::endl;
        close(new_sfd);
        return -1;
    }
}

#elif __linux__

int EventBroker::AcceptConnection(int ident)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    int new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (new_sfd == -1) {
        perror("ERROR: accept() failed to accept a new client");
        return -1;
    }

    struct epoll_event   filter = {};
    filter.events = (EPOLLIN | EPOLLRDHUP);
    filter.data.fd = new_sfd;
    if (epoll_ctl(queue_, EPOLL_CTL_ADD, new_sfd, &filter) == -1) {
        perror("ERROR: epoll_ctl() failed to register a new client in the queue");
        close(new_sfd);
        return -1;
    }

    try
    {
        accepted_sfd_list_.push_back(new_sfd);
        return 0;
    }
    catch(const std::exception& e)
    {
		std::cerr << "ERROR: " << e.what() << std::endl;
        close(new_sfd);
        return -1;
    }
}

#endif

void    EventBroker::DeleteConnection(int ident)
{
    close(ident);
    accepted_sfd_list_.erase(std::find(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), ident));
}

#ifdef __APPLE__

void    EventBroker::SendData(struct kevent* event, char* buf /* replace with future request and response queue */)
{
    // TODO: send the response
    // TODO: for now, just echo back;
    const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
    send(event->ident, response.c_str(), response.size(), 0);
    // TODO: check error here

    // TODO: to remove
    *buf = '\0';
    // TODO: to remove

    // modify the filter to remove EVFILT_WRITE
    // TODO: only if there is no more response to send for this fd
    struct kevent   filter;
    EV_SET(&filter, event->ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(queue_, &filter, 1, NULL, 0, NULL) == -1) {
        perror("ERROR: kevent() failed to delete EVFILT_WRITE filter for a socket");
        // TODO: how to handle this error? Maybe close the connection?
    }
}

#elif __linux__

void    EventBroker::SendData(struct epoll_event* event, char* buf /* replace with future request and response queue */)
{
    // TODO: send the response
    // TODO: for now, just echo back;
    const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
    send(event->data.fd, response.c_str(), response.size(), 0);
    // TODO: check error here

    // TODO: to remove
    *buf = '\0';
    // TODO: to remove

    // modify the filter to remove EPOLLOUT
    // TODO: only if there is no more response to send for this fd
    struct epoll_event   filter = {};
    filter.events = (EPOLLIN | EPOLLRDHUP);
    filter.data.fd = event->data.fd;
    if (epoll_ctl(queue_, EPOLL_CTL_MOD, filter.data.fd, &filter) == -1) {
        perror("ERROR: epoll_ctl() failed to delete EPOLLOUT filter for a socket");
        // TODO: how to handle this error? Maybe close the connection?
    }
}

#endif

#ifdef __APPLE__

void    EventBroker::ReceiveData(struct kevent* event, char* buf /* replace with future request and response queue */)
{
    // TODO: prepare the request or append to complete an incomplete request
    // for now, just consume data
    int bytes_read = recv(event->ident, buf, 100000 - 1, 0 /* MSG_WAITALL */);
    buf[bytes_read] = '\0';

    // modify the filter to add EVFILT_WRITE
    // TODO: only if the request queue is empty for this fd
    struct kevent   filter;
    EV_SET(&filter, event->ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(queue_, &filter, 1, NULL, 0, NULL) == -1) {
        perror("ERROR: kevent() failed to add EVFILT_WRITE filter for a socket");
        // TODO: how to handle this error? Maybe close the connection?
    }
}

#elif __linux__

void    EventBroker::ReceiveData(struct epoll_event* event, char* buf /* replace with future request and response queue */)
{
    // TODO: prepare the request or append to complete an incomplete request
    // for now, just consume data
    int bytes_read = recv(event->data.fd, buf, 100000 - 1, 0 /* MSG_WAITALL */);
    buf[bytes_read] = '\0';

    // modify the filter to add EPOLLOUT
    // TODO: only if the request queue is empty for this fd
    struct epoll_event   filter = {};
    filter.events = (EPOLLIN | EPOLLOUT | EPOLLRDHUP);
    filter.data.fd = event->data.fd;
    if (epoll_ctl(queue_, EPOLL_CTL_MOD, filter.data.fd, &filter) == -1) {
        perror("ERROR: epoll_ctl() failed to add EPOLLOUT filter for a socket");
        // TODO: handle error, how?
    }
}

#endif
