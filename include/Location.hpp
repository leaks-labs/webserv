#ifndef LOCATION_HPP_
# define LOCATION_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <map>

class Location {

    private:
        std::string path;
        std::string root;
        std::string default_file;
        std::string cgi;
        unsigned int methods;
        std::string proxy;
        bool listing;
        std::map<std::string, int(Location::*)(std::string)> set_functions;
    public:
        Location();
        Location(const Location& src);
        Location&   operator=(Location const & rhs);
        ~Location();
        int setValue(std::string key, std::string value);
        int setPath(std::string value);
        int setRoot(std::string value);
        int setDefaultFile(std::string value);
        int setCgi(std::string value);
        int setMethods(std::string value);
        int setProxy(std::string value);
        int setListing(std::string value);
        void print()const;
};

#endif