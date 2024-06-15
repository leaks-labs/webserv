#include "Config.hpp"

std::string http_methods[] = {"GET", "POST", "DELETE"};

t_config_pair config_pairs[] = 
{
    {"port", &Config::setPort}
};

Config::Config(std::string hostname) : 
    host(hostname),
    port(8080),
    errors("/data/errors"),
    default_file("/data/repository.html"),
    bodymax(100000),
    cgi("php"),
    listing(false),
    root("/data"),
    proxymode(false),
    proxy("")
{
    server_names.push_back("webserv");
    for (unsigned long i = 0; i < sizeof(http_methods) / sizeof(http_methods[0]); i++)
        methods.push_back(http_methods[i]);
};

Config::Config(Config const & src)
{
    if(this != &src)
        *this = src;
}

Config & Config::operator=(Config const & src)
{
    if(this != &src)
        *this = src;
    return *this;
}


Config::~Config(){};


int Config::setValue(std::string key, std::string value)
{
    int len;

    len = sizeof(config_pairs) / sizeof(config_pairs[0]);
    for (int i = 0; i < len; i++)
    {
        if(!key.compare(config_pairs[i].key))
            return (this->*config_pairs[i].f)(value);
    }
    return(0);
}

int Config::setPort(std::string value)
{
    std::stringstream ss(value);
    ss >> port;
    if(port < 0 || port > 9999)
        return(1);
    std::cout << "set port: " << port << std::endl;
    return(0);
}

void Config::print() const
{
    std::cout   << "host: " << host << std::endl
                << "port: " << port << std::endl;
}