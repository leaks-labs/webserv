#include "Server.hpp"

#include <sstream>
#include <iostream>
#include <utility>

Server::Server()
    : host_("0.0.0.0"),
      port_("8080"),
      errors_("/data/errors"),
      bodymax_(0),
      addr_(NULL)
{
    server_names_.push_back("webserv");
    AddLocation("/");
    InitSetFunctions();
};

Server::Server(const Server& src) : 
    host_(src.get_host()),
    port_(src.get_port()),
    errors_(src.get_errors()),
    bodymax_(src.get_bodymax()),
    addr_(NULL)
{
    server_names_ = src.get_server_names();
    locations_ = src.get_locations();
    InitSetFunctions();
};

Server::~Server()
{
}

const std::string&  Server::get_host() const
{
    return host_;
}

const std::string&  Server::get_port() const
{
    return port_;
}

const std::string&  Server::get_errors() const
{
    return errors_;
}

int Server::get_bodymax() const
{
    return bodymax_;
}

const std::vector<std::string>& Server::get_server_names() const 
{
    return server_names_;
}

const std::vector<Location>&    Server::get_locations() const
{
    return locations_;   
}

int Server::set_host(const std::string& value)
{
    host_ = value;
    return 0;
}

int Server::set_port(const std::string& value)
{
    port_ = value;
    return 0;
}

int Server::set_errors(const std::string& value)
{
    errors_ = value;
    return 0;
}

int Server::set_bodymax(const std::string& value)
{
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
        if (!(*it >= '0' && *it <= '9')) {   
            std::cerr << "bodymax value shoud be a digit" << std::endl;         
            return 1;
        }
    }
    std::istringstream   ss(value);
    ss >> bodymax_;
    return 0;

    ////////////////////////////
    // TODO: better version ? //
    ////////////////////////////
    // std::istringstream  iss(value);
    // iss >> std::noskipws >> bodymax_;
    // if (!iss.fail() && iss.eof() && (value[0] != '0' || bodymax_ == 0)) {
    //         return 0;
    // std::cerr << "bodymax value shoud be a digit" << std::endl;         
    // return 1;
    //////////////////////////
}

int Server::set_server_names(const std::string& value)
{
    std::string::size_type start = 0;
    std::string::size_type end = value.find(' ');

    while (true) {
        server_names_.push_back(value.substr(start, end - start));
        if (end == std::string::npos)
            break;
        start = end + 1;
        end = value.find(' ', start);
    }
    return 0;
}

void    Server::set_addr(const struct addrinfo* addrinfo)
{
    addr_ = addrinfo;
}

int Server::SetValue(const std::string& key, const std::string& value)
{
    typedef std::map<const std::string, int (Server::*)(const std::string&)>::iterator it;

    it i = set_functions_.find(key);
    return i == set_functions_.end() ? -1 : (this->*(i->second))(value);
}

int Server::AddLocation(const std::string& value)
{
    locations_.push_back(Location());
    locations_.back().set_path(value);
    return 0;
}

int Server::SetLastLocation(const std::string& key, const std::string& value)
{
    return (locations_.back().SetValue(key, value));
}

void    Server::Print() const
{
    std::cout   << "host: " << host_ << std::endl
                << "port: " << port_ << std::endl
                << "errors: " << errors_ << std::endl
                << "bodymax: " << bodymax_ << std::endl;
    for (std::vector<std::string>::const_iterator it=server_names_.begin(); it != server_names_.end(); ++it)
        std::cout << "server_name: " << *it << std::endl;
    for (std::vector<Location>::const_iterator it=locations_.begin(); it != locations_.end(); ++it) {
        std::cout << std::endl;
        it->Print();
    }
}

void    Server::InitSetFunctions()
{
    set_functions_.insert(std::make_pair("host", &Server::set_host));
    set_functions_.insert(std::make_pair("port", &Server::set_port));
    set_functions_.insert(std::make_pair("server_names", &Server::set_server_names));
    set_functions_.insert(std::make_pair("errors", &Server::set_errors));
    set_functions_.insert(std::make_pair("bodymax", &Server::set_bodymax));
}
