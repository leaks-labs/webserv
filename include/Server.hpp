#ifndef SERVER_HPP_
# define SERVER_HPP_

#include "Location.hpp"
#include <map>

class Server
{
    private:
        std::string host;
        std::string port;
        std::vector<std::string> server_names;
        std::string errors;
        int bodymax;
        std::vector<Location*> locations;
        std::map<std::string, int (Server::*)(std::string)> set_functions;
    public:
        Server();
        ~Server();
        Server(const Server& src);
        Server&   operator=(Server const & rhs);
        int setValue(std::string key, std::string value);
        int setHost(std::string value);
        int setPort(std::string value);
        int setServerNames(std::string value);
        int setErrors(std::string value);
        int setBodyMax(std::string value);
        int addLocation(std::string value);
        int setLastLocation(std::string key, std::string value);
        void print()const;
};

typedef struct s_server_set_pair
{
    std::string key;
    int         (Server::*set)(std::string);
} t_server_set_pair;


#endif