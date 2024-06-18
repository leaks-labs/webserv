#ifndef LOCATION_HPP_
# define LOCATION_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>

class Location {

    private:
        std::string default_file;
        std::string cgi;
        std::string root;
        unsigned int methods;
        std::string proxy;
        bool proxymode;
        bool listing;

    public:
        Location();
        Location(const Location& src);
        Location&   operator=(Location const & rhs);
        ~Location();
        int setValue(std::string key, std::string value);
        int setDefaultFile(std::string value);
        int setCgi(std::string value);
        int setRoot(std::string value);
        int setMethods(std::string value);
        int setProxy(std::string value);
        int setListing(std::string value);
};

typedef struct s_location_set_pair
{
    std::string key;
    int         (Location::*set)(std::string);
} t_location_set_pair;

#endif