#include "CgiHandler.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

CgiHandler::CgiHandler(StreamHandler& stream_handler, HttpResponse & response)
    : 
    stream_handler_(stream_handler),
    response_(response),
    stream_(InitPipe())
{
    stream_handler.UnRegister();
    Fork();
}

CgiHandler::~CgiHandler()
{
    CloseFd(pfd_[1]);
}

int  CgiHandler::InitPipe()
{
    if(socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, pfd_) == -1)
        throw std::runtime_error("pipe() failed to create pipe");
    return pfd_[0];
}

void CgiHandler::CloseFd(int fd)
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void CgiHandler::Fork()
{
    int pid;

    pid = fork();
    if(pid == -1)
        throw std::runtime_error("Failed to fork");
    if(pid == 0)
        Exec();
    CloseFd(pfd_[1]);
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
    {
        kill(pid, SIGKILL);
        stream_handler_.Register();
        throw std::runtime_error("Failed to register Cgi Handler");
    }
}

void CgiHandler::Exec()
{
    std::vector<char *> cmd;
    char **c_cmd;
    int err;

    cmd.push_back(const_cast<char*>(response_.get_cgi_path().c_str()));
    cmd.push_back(const_cast<char*>(response_.get_path().c_str()));
    cmd.push_back(const_cast<char*>(response_.get_args().c_str()));
    cmd.push_back(NULL);
    c_cmd = &cmd[0];
    CloseFd(pfd_[0]);
    err = dup2(pfd_[1], 1);
    CloseFd(pfd_[1]);
    if(err == -1)
        throw std::runtime_error("Cgi : dup2 failed " + std::string(strerror(errno)));
    execve(c_cmd[0], c_cmd, NULL);
        throw std::runtime_error("Cgi : Exec failed " + std::string(strerror(errno)));
    std::exit(errno);
}
 
EventHandler::Handle    CgiHandler::get_handle(void) const
{
    return stream_.get_sfd();
}

void    CgiHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER CgiHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type))
    {
        std::cout << "closing cgi" << std::endl;
        ReturnToStreamHandler();
        return;
    }
    if(!EventTypes::IsReadEvent(event_type))
        return;
    std::string r = stream_.Read();
    response_.addToBuffer(r);
}

void        CgiHandler::ReturnToStreamHandler()
{
    response_.RemoveFirstBodyLine();
    response_.set_complete();
    stream_handler_.Register();
    InitiationDispatcher::Instance().RemoveHandler(this);
}

void CgiHandler::HandleTimeout()
{
    
}