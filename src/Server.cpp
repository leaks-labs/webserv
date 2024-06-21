#include "Server.hpp"

#include <sstream>
#include <iostream>

const std::map<const std::string, int (Server::*)(const std::string&)>  Server::set_functions_ = Server::InitSetFunctions();

Server::Server()
    : host_("0.0.0.0"),
      port_("8080"),
      errors_("/data/errors"),
      bodymax_(0),
      server_names_(1, "webserv"),
      addr_(NULL)
{
    AddLocation("/");
}

Server::Server(const Server& src) : 
    host_(src.get_host()),
    port_(src.get_port()),
    errors_(src.get_errors()),
    bodymax_(src.get_bodymax()),
    server_names_(src.get_server_names()),
    locations_(src.get_locations()),
    addr_(NULL)
{
}

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

const std::vector<const std::string>&   Server::get_server_names() const 
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
    std::istringstream  iss(value);
    iss >> std::noskipws >> bodymax_;
    if (!iss.fail() && iss.eof() && (value[0] != '0' || bodymax_ == 0))
        return 0;
    std::cerr << "bodymax value shoud be a digit" << std::endl;         
    return 1;
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
    typedef std::map<const std::string, int (Server::*)(const std::string&)>::const_iterator it;

    it i = set_functions_.find(key);
    return i == set_functions_.end() ? -1 : (this->*(i->second))(value);
}

int Server::AddLocation(const std::string& value)
{
    if (locations_.empty())
        locations_.push_back(Location());
    else
        locations_.push_back(Location(locations_.front()));
    locations_.back().set_path(value);
    return 0;
}

int Server::SetLastLocation(const std::string& key, const std::string& value)
{
    return (locations_.back().SetValue(key, value));
}

void     Server::SetLastLocationStrict(bool value)
{
    locations_.back().set_strict(value);
}

void     Server::PopFirstLocation()
{
    if (!locations_.empty())
        locations_.erase(locations_.begin());
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

const std::map<const std::string, int (Server::*)(const std::string&)>  Server::InitSetFunctions()
{
    std::map<const std::string, int (Server::*)(const std::string&)>    m;
    m["host"] = &Server::set_host;
    m["port"] = &Server::set_port;
    m["server_names"] = &Server::set_server_names;
    m["errors"] = &Server::set_errors;
    m["bodymax"] = &Server::set_bodymax;
    return m;
}
