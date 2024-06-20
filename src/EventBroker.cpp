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

EventBroker::EventBroker(const ListenerList& listeners)
    : listeners_(listeners), queue_(kqueue())
{
    if (queue_ == -1)
        throw std::runtime_error("kqueue() failed to create the queue");
#ifdef __APPLE__
    if (AddReadFilterFromListeners() == -1) {
        close(queue_);
        throw std::runtime_error("kevent() failed to register listeners in the queue");
    }
#elif __linux__
    for (ListenerList::ConstIterator it = listeners_.begin(); it != listeners_.end(); ++it) {
        if (AddReadFilter(it->get_sfd()) == -1) {
            close(queue_);
            throw std::runtime_error("epoll_ctl() failed to register listeners in the queue");
        }
    }
#endif
}

EventBroker::~EventBroker()
{
    std::for_each(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), close);
    close(queue_);
}

int EventBroker::Run()
{
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("ERROR: signal() failed");
        return -1;
    }
    WaitingLoop();
    return 0;
}

#ifdef __APPLE__

int EventBroker::AddReadFilterFromListeners() const
{
    std::vector<Event>              events(listeners_.EnabledListenerCount());
    std::vector<Event>::iterator    it_kevent = events.begin();
    for (ListenerList::ConstIterator it_listener = listeners_.begin(); it_listener != listeners_.end(); ++it_listener, ++it_kevent)
        EV_SET(&(*it_kevent), it_listener->get_sfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    return kevent(queue_, events.data(), events.size(), NULL, 0, NULL);
}

#endif

int EventBroker::AddReadFilter(int fd) const
{
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    return kevent(queue_, &event, 1, NULL, 0, NULL);
#elif __linux__
    event.events = (EPOLLIN | EPOLLRDHUP);
    event.data.fd = fd;
    return epoll_ctl(queue_, EPOLL_CTL_ADD, fd, &event);
#endif
}

int EventBroker::AddWriteFilter(int fd) const
{
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    return kevent(queue_, &event, 1, NULL, 0, NULL);
#elif __linux__
    event.events = (EPOLLIN | EPOLLOUT | EPOLLRDHUP);
    event.data.fd = fd;
    return epoll_ctl(queue_, EPOLL_CTL_MOD, fd, &event);
#endif
}

int EventBroker::DelWriteFilter(int fd) const
{
    Event   event = {};
#ifdef __APPLE__
    EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    return kevent(queue_, &event, 1, NULL, 0, NULL);
#elif __linux__
    event.events = (EPOLLIN | EPOLLRDHUP);
    event.data.fd = fd;
    return epoll_ctl(queue_, EPOLL_CTL_MOD, fd, &event);
#endif
}

int EventBroker::GetIdent(const Event& event) const
{
#ifdef __APPLE__
    return event.ident;
#elif __linux__
    return event.data.fd;
#endif
}

bool    EventBroker::IsEventEOF(const Event& event) const
{
#ifdef __APPLE__
    return (event.flags & EV_EOF);
#elif __linux__
    return (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR));
#endif
}

bool    EventBroker::IsEventRead(const Event& event) const
{
#ifdef __APPLE__
    return (event.filter == EVFILT_READ);
#elif __linux__
    return (event.events & EPOLLIN);
#endif
}

bool    EventBroker::IsEventWrite(const Event& event) const
{
#ifdef __APPLE__
    return (event.filter == EVFILT_WRITE);
#elif __linux__
    return (event.events & EPOLLOUT);
#endif
}

int     EventBroker::WaitForEvents(int queue, std::vector<Event>& event_list, int event_list_size) const
{
#ifdef __APPLE__
    return kevent(queue, NULL, 0, event_list.data(), event_list_size, NULL);
#elif __linux__
    return epoll_wait(queue_, event_list.data(), event_list_size, -1);
#endif
}

void    EventBroker::WaitingLoop()
{
    std::vector<Event>  event_list(kMaxEvents);

    // TODO: to remove, it's just to simulate request and response queue
    char buf[100000];
    *buf = '\0';
    // TODO: to remove, it's just to simulate request and response queue

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        int number_events = WaitForEvents(queue_, event_list, kMaxEvents);
        // TODO: what happens if kevent fails?

        HandleEvents(event_list, number_events, buf);

        // TODO: analyse all complete requests and create responses

    }

    // TODO: clean all pending requests and responses
}

void    EventBroker::HandleEvents(const std::vector<Event>& event_list, int number_events, char* buf)
{
    for (std::vector<Event>::const_iterator event = event_list.begin(); number_events > 0 && g_signal_received == 0; ++event, --number_events) {
        std::cout << "socket: " << event->ident << std::endl;
        if (IsListener(GetIdent(*event))) {
            std::cout << "ENTER: AcceptConnection " << std::endl;
            AcceptConnection(GetIdent(*event));
            // TODO: check error here?
        } else if (IsEventEOF(*event)) {
            std::cout << "ENTER: DeleteConnection " << std::endl;
            DeleteConnection(GetIdent(*event));
            // TODO: check error here?

            // TODO: delete all pending requests and responses for that connection

        } else {
            if (IsEventWrite(*event) /* TODO: && a response is ready for that fd */ && *buf != '\0') {
                std::cout << "ENTER: SendData " << std::endl;
                SendData(*event, buf);
                // TODO: check error here?
            }
            if (IsEventRead(*event)) {
                std::cout << "ENTER: ReceiveData " << std::endl;
                ReceiveData(*event, buf);
                // TODO: check error here?
            }
        }
    }
}

bool    EventBroker::IsListener(int ident) const
{
    for (ListenerList::ConstIterator it = listeners_.begin(); it != listeners_.end(); ++it)
        if (it->get_sfd() == ident)
            return true;
    return false;
}

int EventBroker::AcceptConnection(int ident)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    int new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (new_sfd == -1) {
        perror("ERROR: accept() failed to accept a new client");
        return -1;
    }
    if (AddReadFilter(new_sfd) == -1) {
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

void    EventBroker::DeleteConnection(int ident)
{
    close(ident);
    accepted_sfd_list_.erase(std::find(accepted_sfd_list_.begin(), accepted_sfd_list_.end(), ident));
}

void    EventBroker::SendData(const Event& event, char* buf /* replace with future request and response queue */)
{
    // TODO: send the response
    // TODO: for now, just echo back;
    const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world!";
    send(GetIdent(event), response.c_str(), response.size(), 0);
    // TODO: check error here

    // TODO: to remove
    *buf = '\0';
    // TODO: to remove

    // modify the filter to remove EVFILT_WRITE
    // TODO: only if there is no more response to send for this fd
    if (DelWriteFilter(GetIdent(event)) == -1) {
        perror("ERROR: kevent() failed to delete EVFILT_WRITE filter for a socket");
        // TODO: how to handle this error? Maybe close the connection?
    }
}

void    EventBroker::ReceiveData(const Event& event, char* buf /* replace with future request and response queue */)
{
    // TODO: prepare the request or append to complete an incomplete request
    // for now, just consume data
    int bytes_read = recv(GetIdent(event), buf, 100000 - 1, 0 /* MSG_WAITALL */);
    buf[bytes_read] = '\0';

    // modify the filter to add EVFILT_WRITE
    // TODO: only if the request queue is empty for this fd
    if (AddWriteFilter(GetIdent(event)) == -1) {
        perror("ERROR: kevent() failed to add EVFILT_WRITE filter for a socket");
        // TODO: how to handle this error? Maybe close the connection?
    }
}
