#include "Server.hpp"

#include <iostream>
#include <stdexcept>

const std::map<std::string, void (Server::*)(const std::string&)>   Server::set_functions_ = Server::InitSetFunctions();

Server::Server()
    : host_("0.0.0.0"),
      port_("8080"),
      server_names_(1, "webserv"),
      addr_(NULL)
{
    AddLocation("/");
}

Server::Server(const Server& src) : 
    host_(src.get_host()),
    port_(src.get_port()),
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

const std::vector<std::string>& Server::get_server_names() const 
{
    return server_names_;
}

const std::vector<Location>&    Server::get_locations() const
{
    return locations_;   
}
const struct addrinfo*          Server::get_addr() const
{
    return addr_;
}

void    Server::set_host(const std::string& value)
{
    host_ = value;
}

void    Server::set_port(const std::string& value)
{
    port_ = value;
}

void    Server::set_server_names(const std::string& value)
{
    std::string::size_type  start = 0;
    std::string::size_type  end;
    std::string             res;
    do {
        end = value.find(' ', start);
        res = value.substr(start, end - start);
        if (res.empty())
            break;
        server_names_.push_back(res);
        start = end + 1;
    } while (end != std::string::npos);
    if (end != std::string::npos || res.empty())
        throw std::runtime_error("server_names is invalid");
}

void    Server::set_addr(const struct addrinfo* addrinfo)
{
    addr_ = addrinfo;
}

size_t  Server::ServerNamesCount() const
{
    return server_names_.size();
}

int Server::SetValue(const std::string& key, const std::string& value)
{
    typedef std::map<std::string, void (Server::*)(const std::string&)>::const_iterator it;

    it i = set_functions_.find(key);
    if (set_functions_.find(key) == set_functions_.end())
        return kInvalidKey;
    (this->*(i->second))(value);
    return kValidKey;
}

void    Server::AddLocation(const std::string& value)
{
    if (locations_.empty())
        locations_.push_back(Location());
    else
        locations_.push_back(Location(locations_.front()));
    locations_.back().set_path(value);
}

int Server::SetLastLocation(const std::string& key, const std::string& value)
{
    return locations_.back().SetValue(key, value);
}

void    Server::SetLastLocationStrict(bool value)
{
    locations_.back().set_strict(value);
}

void    Server::PopDefaultServerName()
{
    if (!server_names_.empty())
        server_names_.erase(server_names_.begin());
}

void    Server::Print() const
{
    std::cout   << "host: " << host_ << std::endl
                << "port: " << port_ << std::endl;
    for (std::vector<std::string>::const_iterator it=server_names_.begin(); it != server_names_.end(); ++it)
        std::cout << "server_name: " << *it << std::endl;
    for (std::vector<Location>::const_iterator it=locations_.begin(); it != locations_.end(); ++it) {
        std::cout << std::endl;
        it->Print();
    }
}

const std::map<std::string, void (Server::*)(const std::string&)>   Server::InitSetFunctions()
{
    std::map<std::string, void (Server::*)(const std::string&)> m;
    m["port"] = &Server::set_port;
    m["server_names"] = &Server::set_server_names;
    return m;
}

bool Server::HasServerName(std::string name) const
{
    for (std::vector<std::string>::const_iterator it = server_names_.begin(); it != server_names_.end(); it++)
        if (*it == name)
            return true;
    return false;
}