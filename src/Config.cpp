#include "Config.hpp"

t_config_pair config_pairs[] = 
{
    {"port", &Config::setPort}
};

Config::Config() : port(8080)
{

};

Config::~Config(){};

int Config::loadData(std::string key, std::string value)
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