#ifndef CONFIG_HPP_
# define CONFIG_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>

class Config {

    private:
        std::string host;
        std::string port;
        std::vector<std::string> server_names;
        std::string errors;
        std::string default_file;
        std::string cgi;
        std::string root;
        std::vector<std::string> methods;
        std::string proxy;
        int bodymax;
        bool proxymode;
        bool listing;

    public:
        Config(const Config& src);
        Config&   operator=(Config const & rhs);
        Config(std::string hostname);
        ~Config();
        int setValue(std::string key, std::string value);
        int setPort(std::string value);
        int setError(std::string value);
        void print()const;

};

typedef struct s_config_pair
{
    std::string key;
    int         (Config::*f)(std::string);
} t_config_pair;

#endif