#ifndef LISTENER_LIST_HPP_
# define LISTENER_LIST_HPP_

# include <string>
# include <vector>

# include <netdb.h>

# include "Listener.hpp"

class ListenerList {
    public:
        class   ConstIterator {
            public:
                ConstIterator();
                ConstIterator(std::vector<Listener*>::const_iterator it);

                ~ConstIterator();

                ConstIterator(const ConstIterator& src);
                ConstIterator&  operator=(const ConstIterator& rhs);

                Listener&       operator*();
                Listener*       operator->();
                ConstIterator&  operator++();
                ConstIterator   operator++(int);
                ConstIterator&  operator--();
                ConstIterator   operator--(int);
                bool            operator==(const ConstIterator& rhs) const;
                bool            operator!=(const ConstIterator& rhs) const;

            private:
                std::vector<Listener*>::const_iterator  it_;
        };

        ListenerList();

        ~ListenerList();

        const Listener&  operator[](size_t index) const;

        ConstIterator   begin() const;
        ConstIterator   end() const;

        size_t  EnabledListenerCount() const;
        void    AddDefaultListenerRecords();
        void    AddListenerRecord(const char* ip, const std::string& port);
        void    EnableListeners();

    private:
        static const std::string  kDefaultPort;

        ListenerList(const ListenerList& src);
        ListenerList&   operator=(const ListenerList& rhs);

        bool    IsValidUniqAddr(const struct addrinfo& addr) const;

        struct addrinfo                 hints_;
        std::vector<struct addrinfo*>   listener_records_;
        std::vector<struct addrinfo*>   listener_records_uniq_;
        std::vector<Listener*>          EnabledListeners_;
};

// to remove
// # include <arpa/inet.h>
// # include <iostream>
// to remove

// // to remove
//         void    PrintListenerRecords() const {
//             for (std::vector<struct addrinfo*>::const_iterator it = listener_records_.begin(); it != listener_records_.end(); ++it) {
//                 for (struct addrinfo *res = *it; res != NULL; res = res->ai_next) {
//                     std::cout << "ADDRESS RECORD" << std::endl;
//                     std::cout << "ai_family: " << res->ai_family << std::endl;
//                     std::cout << "ai_socktype: " << res->ai_socktype << std::endl;
//                     std::cout << "ai_protocol: " << res->ai_protocol << std::endl;
//                     if (res->ai_family == PF_INET) {
//                         struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
//                         char ip[INET_ADDRSTRLEN];
//                         std::cout << "ai_addr: " << inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN) << std::endl;
//                     } else if (res->ai_family == PF_INET6) {
//                         struct sockaddr_in6* addr = reinterpret_cast<struct sockaddr_in6*>(res->ai_addr);
//                         char ip[INET6_ADDRSTRLEN];
//                         inet_ntop(AF_INET6, &addr->sin6_addr, ip, INET6_ADDRSTRLEN);
//                         std::cout << "ai_addr: " << ip << std::endl;
//                     }
//                 }
//             }
//         }
// // to remove

#endif  // LISTENER_LIST_HPP_
