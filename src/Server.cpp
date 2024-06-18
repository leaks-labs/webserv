#include "Server.hpp"

t_server_set_pair server_pairs[] = 
{
    {"port", &Server::setPort},
    {"errors", &Server::setErrors}
};

Server::Server() : 
    host("localhost"),
    port("8080"),
    errors("/data/errors"),
    bodymax(100000)
{
    server_names.push_back("webserv");
};


int Server::setValue(std::string key, std::string value)
{
    int size;

    size = sizeof(server_pairs) / sizeof(server_pairs[0]);
    for (int i = 0; i < size; i++)
    {
        if(!key.compare(server_pairs[i].key))
            return (this->*server_pairs[i].set)(value);
    }
    return(0);
}

int Server::setHost(std::string value)
{
    host = value;
    std::cout << "set host: " << value << std::endl;
    return(0);
}

int Server::setPort(std::string value)
{
    port = value;
    std::cout << "set port: " << value << std::endl;
    return(0);
}

int Server::setServerNames(std::string value)
{
    std::cout << "set server names: " << value << std::endl;
    return(0);
}

int Server::setErrors(std::string value)
{
    std::cout << "set error: " << value << "-> shoud check if path exist" <<std::endl;
    errors = value;
    return(0);
}

int Server::setBodyMax(std::string value)
{
    std::cout << "set bodymax: " << value << std::endl;
    return(0);
}

int Server::addLocation(std::string value)
{
    locations.push_back(new Location());
    locations.back()->setRoot(value);
    std::cout << "add location: " << value<< std::endl;
    return(0);
}

int Server::setLastLocation(std::string key, std::string value)
{
    locations.back()->setValue(key, value);
    std::cout << "set last location: " << value << std::endl;
    return(0);
}