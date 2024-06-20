#include "Server.hpp"

Server::Server() : 
    host("0.0.0.0"),
    port("8080"),
    errors("/data/errors"),
    bodymax(0)
{
    server_names.push_back("webserv");
    addLocation("/");
    initSetFunctions();
};

Server::Server(const Server & src) : 
    host(src.getHost()),
    port(src.getPort()),
    errors(src.getErrors()),
    bodymax(src.getBodyMax())
{
    server_names = src.getServerNames();
    locations = src.getLocations();
    initSetFunctions();
};

void Server::initSetFunctions()
{
    try
    {
        set_functions.insert(std::make_pair("host", &Server::setHost));
        set_functions.insert(std::make_pair("port", &Server::setPort));
        set_functions.insert(std::make_pair("server_names", &Server::setServerNames));
        set_functions.insert(std::make_pair("errors", &Server::setErrors));
        set_functions.insert(std::make_pair("bodymax", &Server::setBodyMax));
    }
    catch(const std::exception& e)
    {
        throw(e.what());
    }
    
}

Server::~Server(){};

int Server::setValue(std::string key, std::string value)
{
    typedef std::map<std::string, int (Server::*)(std::string)>::iterator it;

    it i= set_functions.find(key);
    if(i == set_functions.end())
        return(-1);
    return(this->*(i->second))(value);
}

int Server::setHost(std::string value)
{
    host = value;
    return(0);
}

std::string const & Server::getHost()const
{
    return (host);
}

int Server::setPort(std::string value)
{
    port = value;
    return(0);
}

std::string const & Server::getPort()const
{
    return (port);
}

int Server::setServerNames(std::string value)
{
    std::string::size_type start = 0;
    std::string::size_type end = value.find(' ');

    while (1) {
        server_names.push_back(value.substr(start, end - start));
        if(end == std::string::npos)
            break;
        start = end + 1;
        end = value.find(' ', start);
    }
    return(0);
}

std::vector<std::string> const & Server::getServerNames()const
{
    return (server_names);
}

int Server::setErrors(std::string value)
{
    errors = value;
    return(0);
}

std::string const & Server::getErrors()const
{
    return (errors);
}

int Server::setBodyMax(std::string value)
{
    typedef std::string::const_iterator it;
    for(it i = value.begin(); i != value.end(); i++)
    {
        if(!(*i >= '0' && *i <= '9'))
        {   
            std::cerr << "bodymax value shoud be a digit" << std::endl;         
            return (1);
        }
    }
    std::stringstream ss(value);
    ss >> bodymax;
    return(0);
}

int Server::getBodyMax()const
{
    return (bodymax);
}

int Server::addLocation(std::string value)
{
    try
    {
        locations.push_back(Location());
        locations.back().setPath(value);
    }
    catch(const std::exception& e)
    {
        throw e.what();
    }
    return(0);
}

std::vector<Location> const & Server::getLocations()const
{
    return (locations);   
}

int Server::setLastLocation(std::string key, std::string value)
{
    return(locations.back().setValue(key, value));
}

void Server::setAddrInfo(struct addrinfo *addrinfo)
{
    addr = addrinfo;
}

void Server::print()const
{
    std::cout   << "host: " << host << std::endl
                << "port: " << port << std::endl
                << "errors: " << errors << std::endl
                << "bodymax: " << bodymax << std::endl;
    for (std::vector<std::string>::const_iterator it=server_names.begin(); it != server_names.end(); ++it)
    {
        std::cout << "server_name: " << *it << std::endl;
    }
    for(std::vector<Location>::const_iterator it=locations.begin(); it != locations.end(); ++it)
    {
        std::cout << std::endl;
        it->print();
    }
}