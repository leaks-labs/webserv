#include "ConfigLoader.hpp"

ConfigLoader::ConfigLoader(std::ifstream & f) : file(f)
{};

ConfigLoader::~ConfigLoader(){};

int ConfigLoader::load(std::vector<Server*> & servers)
{
    std::string key;
    std::string value;
    int line;
    bool location;
    line = 0;
    while(file)
    {
        line++;
        std::getline(file, key, ' ');
        std::getline(file, value, '\n');
        if(key.length() == 0 || value.length() == 0)
            continue;
        if(!key.compare("#"))
        {
            servers.push_back(new Server());
            location = false;
        }
        if(!key.compare(">"))
        {
            servers.back()->addLocation(value);
            location = false;
        }
        else 
        {
            if(!location && servers.back()->setValue(key, value))
            {
                std::cerr << "Error in config file at line " << line << std::endl;
                return(1);
            }
            if(location && servers.back()->setLastLocation(key, value))
            {
                std::cerr << "Error in config file at line " << line << std::endl;
                return(1);
            }
        }
    }
    return(0);
}