#ifndef EVENT_BROKER_HPP_
#define EVENT_BROKER_HPP_

#include <vector>

#include "Listener.hpp"

class EventBroker {
public:
  EventBroker(const std::vector<Listener *> &listeners);

  ~EventBroker();

  int run();

private:
  static const int kMaxEvents = 32;

  EventBroker();
  EventBroker(const EventBroker &src);
  EventBroker &operator=(const EventBroker &rhs);

  bool IsListener(int ident) const;
  int AcceptConnection(int ident);
  int DeleteConnection(int ident);

  int queue_;
  const std::vector<Listener *> &listeners_;
  std::vector<int> accepted_sfd_list_;
};

#endif // EVENT_BROKER_HPP_