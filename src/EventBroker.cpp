#include "EventBroker.hpp"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include <unistd.h>

// to remove
#include <iostream>
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

EventBroker::EventBroker(const ListenerList& listeners)
    : listeners_(listeners),
      queue_(kqueue())
{
    if (queue_ == -1)
        throw std::runtime_error("kqueue() failed to create the queue");
    if (AddReadFilterFromListeners() == -1) {
        close(queue_);
        throw std::runtime_error("kevent() failed to register listeners in the queue");
    }
}

#elif __linux__

EventBroker::EventBroker(const ListenerList& listeners)
    : listeners_(listeners),
      queue_(epoll_create(1))
{
    if (queue_ == -1)
        throw std::runtime_error("kqueue() failed to create the queue");
    for (ListenerList::ConstIterator it = listeners_.begin(); it != listeners_.end(); ++it) {
        if (AddReadFilter(it->get_sfd()) == -1) {
            close(queue_);
            throw std::runtime_error("epoll_ctl() failed to register listeners in the queue");
        }
    }
}

#endif

EventBroker::~EventBroker()
{
    // TODO: clean all clients in the future ClientList. For now, just close all accepted sockets in the map
    for (std::map<int, int>::const_iterator it = accepted_sfd_list_.begin(); it != accepted_sfd_list_.end(); ++it)
        close(it->first);
    close(queue_);
}

void    EventBroker::Run()
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        throw std::runtime_error("signal() failed: " + std::string(strerror(errno)));
    WaitingLoop();
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

int     EventBroker::WaitForEvents(std::vector<Event>& event_list, int event_list_size) const
{
#ifdef __APPLE__
    return kevent(queue_, NULL, 0, event_list.data(), event_list_size, NULL);
#elif __linux__
    return epoll_wait(queue_, event_list.data(), event_list_size, -1);
#endif
}

void    EventBroker::WaitingLoop()
{
    std::vector<Event>  event_list(kMaxEvents);

    while (g_signal_received == 0) {
        std::cout << "Waiting for events..." << std::endl;
        int number_events = WaitForEvents(event_list, kMaxEvents);
        if (number_events == -1) {
            if (errno != EINTR)
                perror("ERROR: WaitForEvents() failed to wait for events");
            continue;
            // TODO: is it a good idea to continue? Because we may have an infinite loop
        }

        HandleEvents(event_list, number_events);

        // TODO: analyse all complete requests and create responses

    }

    // TODO: clean all pending requests and responses
}

void    EventBroker::HandleEvents(const std::vector<Event>& event_list, int number_events)
{
    for (std::vector<Event>::const_iterator event = event_list.begin(); number_events > 0 && g_signal_received == 0; ++event, --number_events) {
        std::cout << "socket: " << GetIdent(*event) << std::endl;
        if (IsListener(GetIdent(*event))) {
            AcceptConnection(GetIdent(*event));
        } else if (IsEventEOF(*event)) {
            DeleteConnection(GetIdent(*event));

            // TODO: delete all pending requests and responses for that connection

        } else {
            try
            {
                // TODO: SendData if at leat one response is ready for that client. For now, just check the map
                if (IsEventWrite(*event) && accepted_sfd_list_[GetIdent(*event)] > 0)
                    SendData(*event);
                else if (IsEventRead(*event))
                    ReceiveData(*event);
            }
            catch(const std::exception& e)
            {
                std::cerr << "ERROR: " << e.what() << std::endl;;
                DeleteConnection(GetIdent(*event));
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

void    EventBroker::AcceptConnection(int ident)
{
    int new_sfd;
    try
    {
        std::cout << "ENTER: AcceptConnection " << std::endl;
        struct sockaddr_storage addr;
        socklen_t               addr_len = sizeof(addr);
        new_sfd = accept(ident, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
        if (new_sfd == -1)
            throw std::runtime_error("accept() failed to accept a new client: " + std::string(strerror(errno)));
        if (AddReadFilter(new_sfd) == -1)
            throw std::runtime_error("failed to register a new client in the queue: " + std::string(strerror(errno)));
        // TODO: add the new client in the future ClientList. For now, just add client in the map
        accepted_sfd_list_[new_sfd] = 0;
    }
    catch(const std::exception& e)
    {
		std::cerr << "ERROR: " << e.what() << std::endl;
        if (new_sfd != -1)
            close(new_sfd);
    }
}

void    EventBroker::DeleteConnection(int ident)
{
    std::cout << "ENTER: DeleteConnection " << std::endl;
    close(ident);
    accepted_sfd_list_.erase(ident);
}

void    EventBroker::SendData(const Event& event)
{
    try
    {
        
        std::cout << "ENTER: SendData " << std::endl;
        // TODO: send the response
        // TODO: for now, just echo back;
        Response res;
        const std::string response = res.GetText();
        //const std::string   response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
        ssize_t bytes_sent;
        if ((bytes_sent = send(GetIdent(event), response.c_str(), response.size(), 0)) == -1)
            throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
        // TODO: if bytes_sent < response.size(), we need to keep the response in the response queue with the remaining data.
        // or if bytes_sent == response.size(), we need to remove the response from the response queue in the future ClientList.
        // For now, just remove the response from the map.
        --accepted_sfd_list_[GetIdent(event)];

        // modify the filter to remove EVFILT_WRITE
        // only if there is no more response to send for this fd
        // TODO: check if the response queue is empty (COMPLETE or NOT COMPLETE) for this fd. For now, just check the map
        if (accepted_sfd_list_[GetIdent(event)] == 0 && DelWriteFilter(GetIdent(event)) == -1)
            throw std::runtime_error("failed to delete write filter for a socket:" + std::string(strerror(errno)));
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;;
        DeleteConnection(GetIdent(event));
    }
}

void    EventBroker::ReceiveData(const Event& event)
{
    try
    {
        std::cout << "ENTER: ReceiveData " << std::endl;
        char   buf[kBufSize];
        ssize_t bytes_read;
        // TODO: prepare the request or append to complete an incomplete request
        // for now, just consume data
        if ((bytes_read = recv(GetIdent(event), &buf[0], kBufSize - 1, 0)) == -1)
            throw std::runtime_error("recv() failed:" + std::string(strerror(errno)));
        buf[bytes_read] = '\0';
        std::cout << buf << std::endl;
        // TODO: add buf to the request queue

        // TODO: the future ClientList will add the count of VALID requests only if the request is complete
        // For now, just add the request in the map.
        ++accepted_sfd_list_[GetIdent(event)];

        // modify the filter to add EVFILT_WRITE
        // only if the request queue is empty for this fd
        // TODO: check if the request queue is equel to 1 (of COMPLETE and VALID requests) for this fd. For now, just check the map
        if (accepted_sfd_list_[GetIdent(event)] == 1 && AddWriteFilter(GetIdent(event)) == -1)
            throw std::runtime_error("failed to add write filter for a socket:" + std::string(strerror(errno)));
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;;
        DeleteConnection(GetIdent(event));
    }
}
