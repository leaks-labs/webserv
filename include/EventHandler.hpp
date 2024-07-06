#ifndef EVENT_HANDLER_HPP_
# define EVENT_HANDLER_HPP_

# include "EventTypes.hpp"

class EventHandler {
    public:
        typedef int Handle;

        virtual ~EventHandler();

        virtual Handle      get_handle() const = 0;
        EventTypes::Type    get_event_types_registred() const;

        virtual void    HandleEvent(EventTypes::Type event_type) = 0;

        void    AddRegistredEvent(EventTypes::Type event_type);
        void    DelRegistredEvent(EventTypes::Type event_type);

    protected:
        EventHandler();

    private:
        EventHandler(const EventHandler& src);
        EventHandler&   operator=(const EventHandler& rhs);

        EventTypes::Type    event_types_registred_;
};

#endif  // EVENT_HANDLER_HPP_
