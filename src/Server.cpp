#include "Server.hpp"

Server::Server() : 
    host("0.0.0.0"),
    port("8080"),
    errors("/data/errors"),
    bodymax(0)
{
    server_names.push_back("webserv");
    addLocation("/");
    set_functions.insert(std::make_pair("host", &Server::setHost));
    set_functions.insert(std::make_pair("port", &Server::setPort));
    set_functions.insert(std::make_pair("server_names", &Server::setServerNames));
    set_functions.insert(std::make_pair("errors", &Server::setErrors));
    set_functions.insert(std::make_pair("bodymax", &Server::setBodyMax));
};

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

int Server::setPort(std::string value)
{
    port = value;
    return(0);
}

int Server::setServerNames(std::string value)
{
    std::string::size_type start = 0;
    std::string::size_type end = value.find(' ');

    while (end != std::string::npos) {
        server_names.push_back(value.substr(start, end - start));
        start = end + 1;
        end = value.find(' ', start);
    }
    return(0);
}

int Server::setErrors(std::string value)
{
    errors = value;
    return(0);
}

int Server::setBodyMax(std::string value)
{
    typedef std::string::const_iterator it;
    it i = value.begin();
    while(i < value.end())
    {
        if(*i < '0' && *i > '9')
        {   
            std::cout << "bodymax value shoud be a digit" << std::endl;         
            return (1);
        }
        i++;
    }
    std::stringstream ss(value);
    ss >> bodymax;
    return(0);
}

int Server::addLocation(std::string value)
{
    locations.push_back(new Location());
    locations.back()->setPath(value);
    return(0);
}

int Server::setLastLocation(std::string key, std::string value)
{
    locations.back()->setValue(key, value);
    return(0);
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
    for(std::vector<Location*>::const_iterator it=locations.begin(); it != locations.end(); ++it)
    {
        std::cout << std::endl;
        (*it)->print();
    }
}