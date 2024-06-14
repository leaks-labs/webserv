#ifndef CONFIG_HPP_
# define CONFIG_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class Config {

    private:
        Config(const Config& src);
        Config&   operator=(const Config& rhs);
        int port;
        int host[4];
        std::vector<std::string> server_names;
        std::string errors;
        std::string default_file;
        int bodymax;
        std::string cgi;
        bool listing;
        std::string root;
        std::vector<std::string> methods;
        std::string proxy;

    public:
        Config();
        ~Config();
        int loadData(std::string key, std::string value);
        int setPort(std::string value);

};

typedef struct s_config_pair
{
    std::string key;
    int         (Config::*f)(std::string);
} t_config_pair;

#endif