#ifndef SERVER_HPP_
# define SERVER_HPP_

#include "Location.hpp"
#include <map>

class Server
{
    private:
        void initSetFunctions();
        Server&   operator=(Server const & rhs);
        std::string host;
        std::string port;
        std::vector<std::string> server_names;
        std::string errors;
        int bodymax;
        std::vector<Location> locations;
        std::map<std::string, int (Server::*)(std::string)> set_functions;
        struct addrinfo *addr;
    public:
        Server();
        Server(const Server& src);
        ~Server();
        int setValue(std::string key, std::string value);
        int setHost(std::string value);
        int setPort(std::string value);
        int setServerNames(std::string value);
        int setErrors(std::string value);
        int setBodyMax(std::string value);
        int addLocation(std::string value);
        int setLastLocation(std::string key, std::string value);
        void setAddrInfo(struct addrinfo *addr);
        std::string const & getHost()const;
        std::string const & getPort()const;
        std::vector<std::string> const & getServerNames()const;
        std::string const & getErrors()const;
        int getBodyMax()const;
        std::vector<Location> const & getLocations()const;
        void print()const;
};

typedef struct s_server_set_pair
{
    std::string key;
    int         (Server::*set)(std::string);
} t_server_set_pair;


#endif