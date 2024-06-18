#include "Location.hpp"

t_location_set_pair Location_pairs[] = 
{
    {"default_file", &Location::setDefaultFile},
    {"cgi", &Location::setCgi},
};

Location::Location() : 
    default_file("/data/default.html"),
    cgi("php"),
    root("/"),
    methods(7),
    proxymode(false),
    listing(true)
{
};

int Location::setValue(std::string key, std::string value)
{
    int size;

    size = sizeof(Location_pairs) / sizeof(Location_pairs[0]);
    for (int i = 0; i < size; i++)
    {
        if(!key.compare(Location_pairs[i].key))
            return (this->*Location_pairs[i].set)(value);
    }
    return(0);
}

int Location::setDefaultFile(std::string value)
{
    std::cout << "set defaultFile: " << value << std::endl;
    return(0);
}


int Location::setCgi(std::string value)
{
    std::cout << "set Cgi: " << value << std::endl;
    return(0);
}

int Location::setRoot(std::string value)
{
    std::cout << "set Root: " << value << std::endl;
    return(0);
}

int Location::setMethods(std::string value)
{
    std::cout << "set Methods: " << value << std::endl;
    return(0);
}

int Location::setProxy(std::string value)
{
    std::cout << "set Proxy: " << value << std::endl;
    return(0);
}

int Location::setListing(std::string value)
{
    std::cout << "set Listing: " << value << std::endl;
    return(0);
}