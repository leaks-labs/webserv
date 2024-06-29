#include "EventHandler.hpp"

EventHandler::~EventHandler()
{
}

EventTypes::Type    EventHandler::get_event_types_registred() const
{
    return event_types_registred_;
}

void    EventHandler::AddRegistredEvent(EventTypes::Type event_type)
{
    event_types_registred_ |= event_type;
}
void    EventHandler::DelRegistredEvent(EventTypes::Type event_type)
{
    event_types_registred_ &= ~(event_type);
}

EventHandler::EventHandler()
    : event_types_registred_(EventTypes::kNoEvent)
{
}
