#include "EventTypes.hpp"

bool    EventTypes::IsReadEvent(Type type)
{
    return ((type & kReadEvent) != 0);
}

bool    EventTypes::IsWriteEvent(Type type)
{
    return ((type & kWriteEvent) != 0);
}

bool    EventTypes::IsCloseEvent(Type type)
{
    return ((type & kCloseEvent) != 0);
}
