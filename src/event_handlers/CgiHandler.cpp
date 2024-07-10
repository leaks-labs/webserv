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


CgiHandler::CgiHandler(StreamHandler& stream_handler, std::string const & request)
    : stream_handler_(stream_handler),
    stream_(InitPipe()),
    request_(request)
{
    if (InitiationDispatcher::Instance().DeactivateHandler(stream_handler) == -1)
    {
        CloseFd(pfd_[0]);
        CloseFd(pfd_[1]);
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
    }
    Fork();
}


CgiHandler::~CgiHandler()
{
    CloseFd(pfd_[0]);
}


int  CgiHandler::InitPipe()
{
    if(pipe(pfd_)== -1)
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
        ReturnToStreamHandler();
        throw std::runtime_error("Failed to register Cgi Handler");
    }
}

void CgiHandler::Exec()
{
    std::string cgi_path = "/usr/bin/php-cgi";
    std::string path = "/home/toto/Bureau/19/git_webserv/data/file.php";
    std::string args = "arg1=toto";

    std::vector<char *> cmd;
    char **c_cmd;
    int err;

    cmd.push_back(const_cast<char*>(cgi_path.c_str()));
    cmd.push_back(const_cast<char*>(path.c_str()));
    cmd.push_back(const_cast<char*>(args.c_str()));
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
    //std::cout << "ENTER CgiHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type))
    {
        std::cout << "closing cgi" << std::endl;
        ReturnToStreamHandler();
        return;
    }
    else if(!EventTypes::IsReadEvent(event_type))
        return;
    std::string r = stream_.Read();
    if(r.empty())
    {
        ReturnToStreamHandler();
        return;
    }
    response_ += r;
}

void    CgiHandler::HandleTimeout()
{
    // TODO: implement
}

void    CgiHandler::ReturnToStreamHandler()
{
    int err = (InitiationDispatcher::Instance().AddReadFilter(stream_handler_) == -1 || InitiationDispatcher::Instance().AddWriteFilter(stream_handler_) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err != 0)
        throw std::runtime_error("Failed to reactivate stream handler");
}
