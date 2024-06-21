#ifndef SERVER_LIST_HPP_
# define SERVER_LIST_HPP_

# include <fstream>
# include <string>
# include <vector>

# include "Server.hpp"

class ServerList {
    public:
        typedef std::vector<Server>::iterator               Iterator;
        typedef std::vector<Server>::const_iterator         ConstIterator;
        typedef std::vector<Server>::reverse_iterator       ReverseIterator;
        typedef std::vector<Server>::const_reverse_iterator ConstReverseIterator;

        ServerList();

        ~ServerList();

        Server& operator[](size_t index);

        Iterator                begin();
        Iterator                end();
        ConstIterator           begin() const;
        ConstIterator           end() const;
        ReverseIterator         rbegin();
        ReverseIterator         rend();
        ConstReverseIterator    rbegin() const;
        ConstReverseIterator    rend() const;

        size_t                      Size() const;
        void                        AddServer();
        void                        OpenFile(const std::string& path);
        void                        Print() const;
        const std::vector<Server>&  get_servers() const;

    private:
        ServerList(const ServerList& src);
        ServerList&   operator=(const ServerList& rhs);


        int LoadFile();

        std::ifstream       file_;
        std::vector<Server> servers_;
};

#endif  // SERVER_LIST_HPP_
