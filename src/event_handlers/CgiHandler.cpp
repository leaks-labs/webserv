#include "CgiHandler.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "HttpCodeException.hpp"

CgiHandler::CgiHandler(StreamHandler& stream_handler, HttpResponse& response)
    : error_occured_while_handle_event_(false),
      stream_handler_(stream_handler),
      response_(response),
      data_to_send_to_cgi_(response.get_request_body_buffer()),
      sfd_pair_(InitSocketPair()),
      stream_main_(sfd_pair_.first),
      stream_child_(sfd_pair_.second),
      should_return_to_stream_handler_(kContinueCgi),
      pid_child_(fork())
{
    if (pid_child_ == -1)
        throw std::runtime_error("Failed to fork: " + std::string(strerror(errno)));
    if (pid_child_ == 0)
        ExecCGI();
    stream_child_.Close();
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kWriteEvent | EventTypes::kReadEvent) == -1) {
        KillChild();
        throw std::runtime_error("Failed to register Cgi Handler");
    }
    try
    {
        timeout_it_ = InitiationDispatcher::Instance().AddTimeout(this, InitiationDispatcher::kPluginTimeout);
    }
    catch(const std::exception& e)
    {
        KillChild();
        InitiationDispatcher::Instance().RemoveEntry(this);
        throw;
    }
}

CgiHandler::~CgiHandler()
{
    KillChild();
    InitiationDispatcher::Instance().DelTimeout(timeout_it_);
}

EventHandler::Handle    CgiHandler::get_handle(void) const
{
    return stream_main_.get_sfd();
}

int CgiHandler::ReadFromCGI()
{
    std::string res = stream_main_.Read();
    if (res.empty())
        return kReturnToStreamHandler;
    cgi_buffer += res;
    return kContinueCgi;
}

void    CgiHandler::WriteToCGI()
{
    if (!data_to_send_to_cgi_.empty())
        stream_main_.Send(data_to_send_to_cgi_);
    if (data_to_send_to_cgi_.empty() && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
        throw std::runtime_error("Failed to update Cgi Handler filters");
}

void    CgiHandler::HandleEvent(EventTypes::Type event_type)
{
    if (EventTypes::IsCloseReadEvent(event_type)
        || (EventTypes::IsCloseWriteEvent(event_type) && !data_to_send_to_cgi_.empty())) {
        should_return_to_stream_handler_ = kReturnToStreamHandler;
    } else {
        try
        {
            if (EventTypes::IsWriteEvent(event_type))
                WriteToCGI();
            else if (EventTypes::IsReadEvent(event_type))
                should_return_to_stream_handler_ = ReadFromCGI();
        }
        catch(const std::exception& e)
        {
            error_occured_while_handle_event_ = true;
            ReturnToStreamHandler();
            return; // Do NOT remove this return. It is important to be sure to return here.
        }
    }
    if (should_return_to_stream_handler_ == kReturnToStreamHandler) {
        ReturnToStreamHandler();
        return; // Do NOT remove this return. It is important to be sure to return here.
    }
}

void CgiHandler::HandleTimeout()
{
    response_.SetResponseToErrorPage(504);
    int err = InitiationDispatcher::Instance().AddWriteFilter(stream_handler_);
    if (err == -1)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err == -1)
        throw std::runtime_error("Failed to reactivate Stream Handler");
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
        std::string path = response_.get_path();
        size_t pos= path.rfind("/") + 1;
        cmd[1] = const_cast<char*>(path.substr(pos, path.size() - pos).c_str());
        std::vector<char*>::iterator it_dest = env.begin();
        for (std::vector<std::string>::const_iterator it_src = env_src.begin(); it_src != env_src.end(); ++it_src, ++it_dest)
            *it_dest = const_cast<char*>(it_src->c_str());
        err_out = dup2(stream_child_.get_sfd(), STDOUT_FILENO);
        if (err_out != -1)
            err_in = dup2(stream_child_.get_sfd(), STDIN_FILENO);
        stream_child_.Close();
        if (err_in == -1 || err_out == -1)
            throw std::runtime_error("Cgi : dup2 failed " + std::string(strerror(errno)));
        if (chdir(response_.get_path().substr(0, response_.get_path().rfind("/") + 1).c_str()) == -1)
            throw std::runtime_error("Cgi : chdir failed " + std::string(strerror(errno)));
        execve(cmd[0], cmd.data(), env.data());
        throw std::runtime_error("Cgi : execve failed " + std::string(strerror(errno)));
    }
    catch(const std::exception& e)
    {
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
    timeout_it_ = InitiationDispatcher::Instance().DelTimeout(timeout_it_);
    int err = 0;
    int status_err;
    typedef std::map<std::string, std::string>::const_iterator iterator;
    
    try
    {
        if (error_occured_while_handle_event_)
            throw HttpCodeExceptions::InternalServerErrorException();
        response_.ClearHeader();
        response_.ParseHeader(cgi_buffer);
        if (!response_.HeaderIsComplete())
            throw HttpCodeExceptions::BadGatewayException();
        iterator it = response_.get_header().get_header_map().find("STATUS");
        if (it != response_.get_header().get_header_map().end() 
            && (status_err = ExtractStatusError(it->second)) >= 400)
            response_.SetResponseToErrorPage(status_err);
        else{
            response_.set_body(cgi_buffer);
            response_.AddHeaderContentLength();
            response_.set_status_line(status_err);
            response_.SetComplete();
            err = InitiationDispatcher::Instance().AddWriteFilter(stream_handler_);
        }
    }
    catch(const HttpCodeException& e)
    {
        try
        {
            response_.SetResponseToErrorPage(e.Code());
        }
        catch(const std::exception& e)
        {
            err = -1;
        }
    }
    catch(const std::exception& e)
    {
        try
        {
            response_.SetResponseToErrorPage(500);
        }
        catch(const std::exception& e)
        {
            err = -1;
        }
    }
    if (err == -1)
        InitiationDispatcher::Instance().RemoveHandler(&stream_handler_);
    InitiationDispatcher::Instance().RemoveHandler(this);
    if (err == -1)
        throw std::runtime_error("Failed to return to Stream Handler");
}

int CgiHandler::ExtractStatusError(std::string str)
{
    int err;
    std::stringstream ss(str.substr(0, 3));
    ss >> err;
    return err;
}