#include "CgiHandler.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <sys/wait.h>

#include "InitiationDispatcher.hpp"

// TODO: to remove
#include <iostream>
// TODO: to remove

CgiHandler::CgiHandler(StreamHandler& stream_handler, HttpResponse& response)
    : stream_handler_(stream_handler),
      response_(response),
      sfd_pair_(InitSocketPair()),
      stream_main_(sfd_pair_.first),
      stream_child_(sfd_pair_.second),
      pid_child_(fork())
{
    if (pid_child_ == -1)
        throw std::runtime_error("Failed to fork: " + std::string(strerror(errno)));
    if (pid_child_ == 0)
        Exec();
    stream_child_.Close();
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1) {
        KillChild();
        throw std::runtime_error("Failed to register Cgi Handler");
    }
    if (stream_handler.UnRegister() == -1) {
        KillChild();
        InitiationDispatcher::Instance().RemoveEntry(this);
        throw std::runtime_error("Failed to unregister Stream Handler");
    }
    // TODO: or update the response with a 500 error or something instead of throwing?
}

CgiHandler::~CgiHandler()
{
    KillChild();
}

EventHandler::Handle    CgiHandler::get_handle(void) const
{
    return stream_main_.get_sfd();
}

void    CgiHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER CgiHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing cgi" << std::endl;
        // TODO: either close this handler because all is alright or update the response with a 500 error or something,
        // because the result is not complete and that an error occured
        ReturnToStreamHandler();
        return; // Do NOT remove this return. It is important to be sure to return here.
    } else if (EventTypes::IsReadEvent(event_type)) {
        std::string r = stream_main_.Read();
        // TODO: what happens if Read() throws?
        response_.AddToBody(r);
    }
}

void CgiHandler::HandleTimeout()
{
    // TODO: implement
}

std::pair<int, int> CgiHandler::InitSocketPair()
{
    int pfd[2];
#ifdef __APPLE__
    if(socketpair(PF_UNIX, SOCK_STREAM, 0, pfd) == -1)
        throw std::runtime_error("socketpair() failed");
    if (fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1 
        || fcntl(pfd[0], F_SETFD, FD_CLOEXEC) == -1
        || fcntl(pfd[1], F_SETFL, O_NONBLOCK) == -1
        || fcntl(pfd[1], F_SETFD, FD_CLOEXEC) == -1)
        throw std::runtime_error("fcntl() failed");
#elif __linux__
    if(socketpair(PF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, pfd) == -1)
        throw std::runtime_error("socketpair() failed");
#endif
    return std::pair<int, int>(pfd[0], pfd[1]);
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
    stream_main_.Close();
    err = dup2(stream_child_.get_sfd(), 1);
    stream_child_.Close();
    if(err == -1)
        throw std::runtime_error("Cgi : dup2 failed " + std::string(strerror(errno)));
    execve(c_cmd[0], c_cmd, NULL);
        throw std::runtime_error("Cgi : Exec failed " + std::string(strerror(errno)));
    std::exit(errno);
}
 
void    CgiHandler::KillChild()
{
    int status;
    if (waitpid(pid_child_, &status, WNOHANG) == 0)
        if (kill(pid_child_, SIGKILL) == -1)
            perror("kill");
}

void    CgiHandler::ReturnToStreamHandler()
{
    // TODO: or update the response with a 500 error or something, if the body is not valid
    response_.CgiParseHeader();
    response_.SetComplete();
    int err = stream_handler_.ReRegister();
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err == -1)
        throw std::runtime_error("Failed to reactivate Stream Handler");
}
