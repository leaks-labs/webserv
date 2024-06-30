#ifndef EVENT_TYPES_HPP_
# define EVENT_TYPES_HPP_

namespace EventTypes
{

typedef int Type;

static const int    kNoEvent = 0;
static const int    kReadEvent = 1 << 0;
static const int    kWriteEvent = 1 << 1;
static const int    kCloseEvent = 1 << 2;

bool    IsReadEvent(Type type);
bool    IsWriteEvent(Type type);
bool    IsCloseEvent(Type type);

}

#endif  // EVENT_TYPES_HPP_
