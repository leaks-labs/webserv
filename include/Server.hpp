#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <map>
# include <string>
# include <vector>
# include <netdb.h>

# include "Location.hpp"

class Server {
    public:
        static const int    kInvalidKey = -1;
        static const int    kValidKey = 0;

        Server();
        Server(const Server& src);

        ~Server();

        const std::string&              get_host() const;
        const std::string&              get_port() const;
        const std::vector<std::string>& get_server_names() const;
        const std::vector<Location>&    get_locations() const;
        const struct addrinfo*          get_addr() const;
        void    set_host(const std::string& value);
        void    set_port(const std::string& value);
        void    set_server_names(const std::string& value);
        void    set_addr(const struct addrinfo* addr);

        size_t  ServerNamesCount() const;
        int     SetValue(const std::string& key, const std::string& value);
        void    AddLocation(const std::string& value);
        int     SetLastLocation(const std::string& key, const std::string& value);
        void    SetLastLocationStrict(bool value);
        void    PopDefaultServerName();
        void    Print() const;
        bool    HasServerName(std::string) const;
        const   Location* FindLocation(std::string const &) const; 
    private:
        Server& operator=(const Server& rhs);

        static const std::map<std::string, void (Server::*)(const std::string&)>    set_functions_;

        static const std::map<std::string, void (Server::*)(const std::string&)>    InitSetFunctions();

        std::string                 host_;
        std::string                 port_;
        std::vector<std::string>    server_names_;
        std::vector<Location>       locations_;
        const struct addrinfo*      addr_;
};

#endif // SERVER_HPP_
