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
    int err = (InitiationDispatcher::Instance().AddReadFilter(*this) == -1 
        || InitiationDispatcher::Instance().AddWriteFilter(*this) == -1);
    if (err != 0)
        InitiationDispatcher::Instance().RemoveHandler(this);
}

void StreamHandler::UnRegister()
{
    if (InitiationDispatcher::Instance().DeactivateHandler(*this) == -1)
        throw std::runtime_error("Failed to deactivate StreamHandler with InitiationDispatcher");
}

EventHandler::Handle    StreamHandler::get_handle() const
{
    return stream_.get_sfd();
}

void    StreamHandler::HandleEvent(EventTypes::Type event_type)
{
    std::cout << "ENTER StreamHandler: event " << event_type << std::endl;
    if (EventTypes::IsCloseEvent(event_type)) {
        std::cout << "closing stream" << std::endl;
        InitiationDispatcher::Instance().RemoveHandler(this);
        return; // Do NOT remove this return. It is important to be sure to return here.
    } else {
        try
        {
            if (EventTypes::IsWriteEvent(event_type) && !response_queue_.empty()) {
                // TODO: send the first compliete response. For now, just send a string.

                // TODO: if there is bytes not sent, we need to keep the response in the response queue with the remaining data.
                // or if all bytes sent, we need to remove the response from the response queue.
                // For now, just remove the response from the map.
                Encode();
                // modify the filter to remove write filter
                // only if there is no more response to send for this fd
                // TODO: check if the response queue is empty (COMPLETE or NOT COMPLETE) for this fd. For now, just check the size_t
                if (response_queue_.empty() && InitiationDispatcher::Instance().DelWriteFilter(*this) == -1)
                    throw std::runtime_error("Failed to delete write filter for a socket");
            } else if (EventTypes::IsReadEvent(event_type)) {
                // TODO: add the string return by Read to the request queue
                // For now, just add the request in the map.
                std::string r = stream_.Read();
                Decode(r);
                
                // modify the filter to add EVFILT_WRITE
                // only if the request queue is empty for this fd
                // TODO: check if the request queue is equel to 1 (of COMPLETE and VALID requests) for this fd. For now, just check the size_t
                if (!response_queue_.empty() && InitiationDispatcher::Instance().AddWriteFilter(*this) == -1)
                    throw std::runtime_error("failed to add write filter for a socket:" + std::string(strerror(errno)));
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
}


void    StreamHandler::Encode()
{
    HttpResponse response = *response_queue_.back();
    if(response.get_complete())
    {
        std::string res = response.get_content();
        stream_.Send(res);
        delete response_queue_.back();
        response_queue_.pop_back();
    }
}