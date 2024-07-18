#include "EventTypes.hpp"

bool    EventTypes::IsReadEvent(Type type)
{
    return ((type & kReadEvent) != 0);
}

bool    EventTypes::IsWriteEvent(Type type)
{
    return ((type & kWriteEvent) != 0);
}

bool    EventTypes::IsCloseReadEvent(Type type)
{
    return ((type & kCloseReadEvent) != 0);
}

bool    EventTypes::IsCloseWriteEvent(Type type)
{
    return ((type & kCloseWriteEvent) != 0);
}
