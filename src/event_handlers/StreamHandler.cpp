#include "StreamHandler.hpp"

#include <stdexcept>

StreamHandler::StreamHandler(int acceptor_sfd, int sfd)
    : acceptor_sfd_(acceptor_sfd),
      stream_(sfd),
      should_close_connection_(kKeepConnection)
{
    if (InitiationDispatcher::Instance().RegisterHandler(this, EventTypes::kReadEvent) == -1)
        throw std::runtime_error("Failed to register StreamHandler with InitiationDispatcher");
    try
    {
        timeout_it_ = InitiationDispatcher::Instance().AddTimeout(this, InitiationDispatcher::kRequestTimeout);
    }
    catch(const std::exception& e)
    {
        InitiationDispatcher::Instance().RemoveEntry(this);
        throw;
    }
}

StreamHandler::~StreamHandler()
{
    InitiationDispatcher::Instance().DelTimeout(timeout_it_);
}

EventHandler::Handle    StreamHandler::get_handle() const
{
    return stream_.get_sfd();
}

void    StreamHandler::HandleEvent(EventTypes::Type event_type)
{
    try
    {
        if (EventTypes::IsCloseWriteEvent(event_type) || (EventTypes::IsCloseReadEvent(event_type) && response_queue_.empty()))
            should_close_connection_ = kCloseConnection;
        else if (EventTypes::IsWriteEvent(event_type) && !response_queue_.empty() && response_queue_.front().IsComplete())
            should_close_connection_ = SendFirstResponse();
        else if (EventTypes::IsReadEvent(event_type))
            should_close_connection_ = AddToRequestQueue();
        if (should_close_connection_ == kCloseConnection) {
            InitiationDispatcher::Instance().RemoveHandler(this);
            return; // Do NOT remove this return. It is important to be sure to return here.
        }
        if (!request_queue_.empty() && request_queue_.front().IsComplete() && response_queue_.empty())
            ConvertRequestToResponse();
    }
    catch(const std::exception& e)
    {
        InitiationDispatcher::Instance().RemoveHandler(this);
        throw; // Do NOT remove this throw. It is important to be sure to return here.
    }
}

void    StreamHandler::HandleTimeout()
{
    if (response_queue_.empty()) {
        timeout_it_ = InitiationDispatcher::Instance().DelTimeout(timeout_it_);
        request_queue_.push_front(HttpRequest(acceptor_sfd_, 408));
        response_queue_.push_front(HttpResponse(*this, request_queue_.front()));
        response_queue_.front().Execute();
    }
}

bool    StreamHandler::AddToRequestQueue()
{
    std::string r = stream_.Read();
    if (r.empty() && response_queue_.empty())
        return kCloseConnection;
    while (!r.empty()) {
        if (request_queue_.empty() || request_queue_.back().IsComplete())
            request_queue_.push_back(HttpRequest(acceptor_sfd_));
        request_queue_.back().AppendToRequest(r);
    }
    return kKeepConnection;
}

bool    StreamHandler::SendFirstResponse()
{
    stream_.Send(response_queue_.front().get_response_buffer());
    if (response_queue_.front().get_response_buffer().empty()) {
        if (response_queue_.front().IsAskingToCloseConnection())
            return kCloseConnection;
        response_queue_.pop_front();
        request_queue_.pop_front();
        if (InitiationDispatcher::Instance().SwitchFromWriteToRead(*this) == -1)
            throw std::runtime_error("Failed to delete write filter and add read filter for a socket");
        if (request_queue_.empty() || !request_queue_.front().IsComplete())
            timeout_it_ = InitiationDispatcher::Instance().AddTimeout(this, InitiationDispatcher::kRequestTimeout);
    }
    return kKeepConnection;
}

void    StreamHandler::ConvertRequestToResponse()
{
    timeout_it_ = InitiationDispatcher::Instance().DelTimeout(timeout_it_);
    if (InitiationDispatcher::Instance().DeactivateHandler(*this) == -1)
        throw std::runtime_error("Failed to delete Read filter for a socket");
    response_queue_.push_back(HttpResponse(*this, request_queue_.front()));
    response_queue_.front().Execute();
}
