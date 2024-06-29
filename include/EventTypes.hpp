#ifndef EVENT_TYPES_HPP_
# define EVENT_TYPES_HPP_

namespace EventTypes
{

    typedef int Type;

    static int  kNoEvent = 0;
    static int  kReadEvent = 1 << 0;
    static int  kWriteEvent = 1 << 1;
    static int  kCloseEvent = 1 << 2;

}

#endif  // EVENT_TYPES_HPP_
