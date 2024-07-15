#include "CgiHandler.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

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
        ExecCGI();
    stream_child_.Close();
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kWriteEvent) == -1) {
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
    try
    {
        std::cout << "ENTER CgiHandler: event " << event_type << std::endl;
        if (EventTypes::IsCloseEvent(event_type)) {
            std::cout << "closing cgi" << std::endl;
            // TODO: either close this handler because all is alright or update the response with a 500 error or something,
            // because the result is not complete and that an error occured
            ReturnToStreamHandler();
            return; // Do NOT remove this return. It is important to be sure to return here.
        } else if (EventTypes::IsReadEvent(event_type)) {
            std::string res = stream_main_.Read();
            // TODO: what happens if Read() throws?
            cgi_buffer += res;
        } else if (EventTypes::IsWriteEvent(event_type)) {
            std::string& body = response_.get_request_body();
            if(!body.empty())
                stream_main_.Send(body);
            else
            {
                if (InitiationDispatcher::Instance().DelWriteFilter(*this) == -1
                    || InitiationDispatcher::Instance().AddReadFilter(*this) == -1)
                    throw std::runtime_error("Failed to update Cgi Handler filters");
            }
        }
    }
    catch(const std::exception& e)
    {
        ReturnToStreamHandler();
        return; // Do NOT remove this return. It is important to be sure to return here.
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
        // || fcntl(pfd[1], F_SETFD, FD_CLOEXEC) == -1
        || fcntl(pfd[1], F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl() failed");
#elif __linux__
    if(socketpair(PF_UNIX, SOCK_STREAM | SOCK_NONBLOCK /* | SOCK_CLOEXEC */, 0, pfd) == -1)
        throw std::runtime_error("socketpair() failed");
#endif
    return std::pair<int, int>(pfd[0], pfd[1]);
}

void CgiHandler::ExecCGI()
{
    int err_out = -1, err_in = -1;
    try
    {
        stream_main_.Close();
        std::vector<char*>  cmd(3);
        const std::vector<std::string>&  env_src = response_.get_env();
        std::vector<char*>  env(env_src.size() + 1);
        cmd[0] = const_cast<char*>(response_.get_cgi_path().c_str());
        cmd[1] = const_cast<char*>(response_.get_query().c_str());
        for (std::vector<char*>::const_iterator cmd_it = cmd.begin(); cmd_it != cmd.end(); ++cmd_it)
            std::cout << *cmd_it << std::endl;
        std::vector<char*>::iterator it_dest = cmd.begin();
        for (std::vector<std::string>::const_iterator it_src = env_src.begin(); it_src != env_src.end(); ++it_src, ++it_dest)
            *it_dest = const_cast<char*>(it_src->c_str());
        err_out = dup2(stream_child_.get_sfd(), STDOUT_FILENO);
        if (err_out != -1)
            err_in = dup2(stream_child_.get_sfd(), STDIN_FILENO);
        stream_child_.Close();
        if (err_in == -1 || err_out == -1)
            throw std::runtime_error("Cgi : dup2 failed " + std::string(strerror(errno)));
        // TODO: chdir to the root or to the directory where the .php is?
        if (chdir(response_.get_location().get_root().c_str()) == -1)
            throw std::runtime_error("Cgi : chdir failed " + std::string(strerror(errno)));
        execve(cmd[0], cmd.data(), env.data());
        throw std::runtime_error("Cgi : execve failed " + std::string(strerror(errno)));
        // if execve failed, the content of the socket will be empty
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        stream_child_.Close();
        if (err_out != -1)
            close(STDOUT_FILENO);
        if (err_in != -1)
            close(STDIN_FILENO);
        std::exit(errno);
    }
}
 
void    CgiHandler::KillChild()
{
    int status;
    if (waitpid(pid_child_, &status, WNOHANG) == 0 && kill(pid_child_, SIGKILL) == -1)
        perror("kill");
}

void    CgiHandler::ReturnToStreamHandler()
{
    // TODO: or update the response with a 500 error or something, if the body is not valid
    // TODO: for now just presume all is ok
    response_.set_status_line("HTTP/1.1 200 OK");
    size_t  pos = cgi_buffer.find("\r\n\r\n");
    response_.AppendToHeader(cgi_buffer.substr(0, pos + 2));
    cgi_buffer.erase(0, pos + 4);
    response_.set_body(cgi_buffer);
    response_.AddHeaderContentLength();
    response_.SetComplete();
    int err = stream_handler_.ReRegister();
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err == -1)
        throw std::runtime_error("Failed to reactivate Stream Handler");
}
