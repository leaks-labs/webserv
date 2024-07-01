#include "EventTypes.hpp"

bool    EventTypes::IsReadEvent(Type type)
{
    return (type & kReadEvent);
}

bool    EventTypes::IsWriteEvent(Type type)
{
    return (type & kWriteEvent);
}

bool    EventTypes::IsCloseEvent(Type type)
{
    return (type & kCloseEvent);
}
