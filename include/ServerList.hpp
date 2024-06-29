#ifndef SERVER_LIST_HPP_
# define SERVER_LIST_HPP_

# include <string>
# include <vector>

# include "Server.hpp"

class ServerList {
    public:
        typedef std::vector<Server>::iterator               Iterator;
        typedef std::vector<Server>::const_iterator         ConstIterator;
        typedef std::vector<Server>::reverse_iterator       ReverseIterator;
        typedef std::vector<Server>::const_reverse_iterator ConstReverseIterator;

        static ServerList&  Instance();

        Server& operator[](size_t index);

        Iterator                begin();
        Iterator                end();
        ConstIterator           begin() const;
        ConstIterator           end() const;
        ReverseIterator         rbegin();
        ReverseIterator         rend();
        ConstReverseIterator    rbegin() const;
        ConstReverseIterator    rend() const;

        const std::vector<Server>&  get_servers() const;

        size_t  Size() const;
        void    InitServerList(const std::string& path);
        void    Print() const;

    private:
        ServerList();

        ServerList(const ServerList& src);
        ServerList&   operator=(const ServerList& rhs);

        ~ServerList();

        void    ParseConfigFile(std::ifstream& file);

        std::vector<Server> servers_;
};

#endif  // SERVER_LIST_HPP_
