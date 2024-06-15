#ifndef CONFIG_HPP_
# define CONFIG_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

class Config {

    private:
        std::string host;
        int port;
        std::vector<std::string> server_names;
        std::string errors;
        std::string default_file;
        int bodymax;
        std::string cgi;
        bool listing;
        std::string root;
        std::vector<std::string> methods;
        bool proxymode;
        std::string proxy;

    public:
        Config(const Config& src);
        Config&   operator=(Config const & rhs);
        Config(std::string hostname);
        ~Config();
        int load(std::ifstream & file);
        int loadline(std::string line);
        int setValue(std::string key, std::string value);
        int setPort(std::string value);
        void print()const;

};

typedef struct s_config_pair
{
    std::string key;
    int         (Config::*f)(std::string);
} t_config_pair;

#endif