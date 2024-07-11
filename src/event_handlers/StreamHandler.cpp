#include "StreamHandler.hpp"
#include "HttpResponse.hpp"


StreamHandler::StreamHandler(int acceptor_sfd, int sfd)
    : 
    acceptor_sfd_(acceptor_sfd),
    stream_(sfd)
{
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
        throw std::runtime_error("Failed to register StreamHandler with InitiationDispatcher");
}

StreamHandler::~StreamHandler()
{
}

void StreamHandler::Register()
{
    if(InitiationDispatcher::Instance().AddReadFilter(*this) == -1 
        || InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
        InitiationDispatcher::Instance().RemoveHandler(this);
    else
        std::cout << "StreamHandler is registered" << std::endl;
}

void StreamHandler::UnRegister()
{
    if (InitiationDispatcher::Instance().DeactivateHandler(*this) == -1)
    {
        std::cout << "Failed to deactivate StreamHandler with InitiationDispatcher" << std::endl;
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
    }
    std::cout << "StreamHandler is unregistered" << std::endl;
}

EventHandler::Handle    StreamHandler::get_handle() const
{
    return stream_.get_sfd();
}

void    StreamHandler::HandleEvent(EventTypes::Type event_type)
{
    //std::cout << "ENTER StreamHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing stream" << std::endl;
        InitiationDispatcher::Instance().RemoveHandler(this);
        return; // Do NOT remove this return. It is important to be sure to return here.
    } else {
        try
        {
            if (EventTypes::IsWriteEvent(event_type) && !response_queue_.empty()) {
                Encode();

            } else if (EventTypes::IsReadEvent(event_type)) {
                std::string r = stream_.Read();
                Decode(r);
            }
        }
        catch(const std::exception& e)
        {
            InitiationDispatcher::Instance().RemoveHandler(this);
            throw;
        }
    }
}

void    StreamHandler::HandleTimeout()
{
    // TODO: implement
}

void   StreamHandler::Decode(std::string& buffer)
{
    size_t i = HttpRequest::FindRequest(buffer, 0);
    HttpRequest request;
    request.set_message(buffer.substr(0, i));
    request.Parse();
    response_queue_.push_back(new HttpResponse(*this, request, acceptor_sfd_));
    //if (!response_queue_.empty() && InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
    //    throw std::runtime_error("failed to add write filter for a socket:" + std::string(strerror(errno)));
}

void    StreamHandler::Encode()
{
    HttpResponse response = *response_queue_.back();
    if(response.get_complete())
    {
        stream_.Send(response.get_buffer());
        delete response_queue_.back();
        response_queue_.pop_back();
    }
    if (response_queue_.empty() && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
        throw std::runtime_error("Failed to delete write filter for a socket");
    //else
    //    std::cout << "response is not complete" << std::endl;
}