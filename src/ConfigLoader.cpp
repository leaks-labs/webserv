#include "ConfigLoader.hpp"

ConfigLoader::ConfigLoader(std::ifstream & f) : file(f)
{};

ConfigLoader::~ConfigLoader(){};

int ConfigLoader::load(std::vector<Server*> & servers)
{
    std::string line;
    std::string key;
    std::string value;
    int count;
    int err;
    int sep;

    count = 0;
    while(file)
    {
        count++;
        std::getline(file, line, '\n');
        sep = line.find(' ');
        key = line.substr(0, sep);
        value = line.substr(sep + 1, line.length());
        if(key.length() == 0 || value.length() == 0)
            continue;
        if(!key.compare("#"))
        {
            servers.push_back(new Server());
            if(value.length() > 0)
                servers.back()->setHost(value);
        }
        else if(!key.compare(">"))
        {
            servers.back()->addLocation(value);
        }
        else 
        {
            err = servers.back()->setValue(key, value);
            if (err == -1)
                err = servers.back()->setLastLocation(key, value);
            if(err)
                return(1);
        }
    }
    for (std::vector<Server*>::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        (*it)->print();
    }
    return(0);
}