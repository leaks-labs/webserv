#ifndef LISTENER_LIST_INFO_HPP_
#define LISTENER_LIST_INFO_HPP_

#include <string>
#include <vector>

#include <netdb.h>

#include "Listener.hpp"

// to remove
// # include <arpa/inet.h>
// # include <iostream>
// to remove

class ListenerListInfo {
public:
  ListenerListInfo();

  ~ListenerListInfo();

  void AddDefaultsRecords();
  void AddRecords(const char *ip, const std::string &port);
  void CreateListeners(std::vector<Listener *> &listeners);

  // // to remove
  //         void    PrintListenerRecords() const {
  //             for (std::vector<struct addrinfo*>::const_iterator it =
  //             listener_records_.begin(); it != listener_records_.end(); ++it)
  //             {
  //                 for (struct addrinfo *res = *it; res != NULL; res =
  //                 res->ai_next) {
  //                     std::cout << "ADDRESS RECORD" << std::endl;
  //                     std::cout << "ai_family: " << res->ai_family <<
  //                     std::endl; std::cout << "ai_socktype: " <<
  //                     res->ai_socktype << std::endl; std::cout <<
  //                     "ai_protocol: " << res->ai_protocol << std::endl; if
  //                     (res->ai_family == PF_INET) {
  //                         struct sockaddr_in* addr = reinterpret_cast<struct
  //                         sockaddr_in*>(res->ai_addr); char
  //                         ip[INET_ADDRSTRLEN]; std::cout << "ai_addr: " <<
  //                         inet_ntop(AF_INET, &addr->sin_addr, ip,
  //                         INET_ADDRSTRLEN) << std::endl;
  //                     } else if (res->ai_family == PF_INET6) {
  //                         struct sockaddr_in6* addr = reinterpret_cast<struct
  //                         sockaddr_in6*>(res->ai_addr); char
  //                         ip[INET6_ADDRSTRLEN]; inet_ntop(AF_INET6,
  //                         &addr->sin6_addr, ip, INET6_ADDRSTRLEN); std::cout
  //                         << "ai_addr: " << ip << std::endl;
  //                     }
  //                 }
  //             }
  //         }
  // // to remove

private:
  static const std::string kDefaultPort;

  ListenerListInfo(const ListenerListInfo &src);
  ListenerListInfo &operator=(const ListenerListInfo &rhs);

  bool IsValidUniqAddr(struct addrinfo *addr) const;

  struct addrinfo hints_;
  std::vector<struct addrinfo *> listener_records_;
  std::vector<struct addrinfo *> listeners_addr_uniq;
};

#endif // LISTENER_LIST_INFO_HPP_