#ifndef SERVER_LIST_HPP_
# define SERVER_LIST_HPP_

# include <cstring>
# include <vector>
# include <fstream>
# include <sstream>
# include <stdexcept>
# include "Server.hpp"

class ServerList {
    public:
        typedef std::vector<Server>::iterator               Iterator;
        typedef std::vector<Server>::const_iterator         ConstIterator;
        typedef std::vector<Server>::reverse_iterator       ReverseIterator;
        typedef std::vector<Server>::const_reverse_iterator ConstReverseIterator;

        static ServerList&  Instance();
        static int          IsSameAddr(const int listener_sfd, const struct addrinfo* addr_list);
        static bool         CmpAddr(const struct addrinfo& addr1, const struct addrinfo& addr2);
        static bool         CmpSockAddr(const struct sockaddr& sa1, const struct sockaddr& sa2);

        Server& operator[](size_t index);

        Iterator                Begin();
        Iterator                End();
        ConstIterator           Begin() const;
        ConstIterator           End() const;
        ReverseIterator         Rbegin();
        ReverseIterator         Rend();
        ConstReverseIterator    Rbegin() const;
        ConstReverseIterator    Rend() const;

        size_t          Size() const;
        void            InitServerList(const std::string& path);
        const Server&   FindServer(const int acceptor_sfd, const std::string& name) const;
        void            Print() const;

    private:
        ServerList();

        ServerList(const ServerList& src);
        ServerList&   operator=(const ServerList& rhs);

        ~ServerList();

        void    ParseConfigFile(std::ifstream& file);

        std::vector<Server> servers_;
};

#endif  // SERVER_LIST_HPP_
