#include "ServerList.hpp"


ServerList::ServerList(){};

ServerList::~ServerList(){};

void ServerList::addServer()
{
    try
    {
        servers.push_back(Server());
    }
    catch(const std::exception& e)
    {
        throw e.what();
    }

}

void ServerList::openFile(std::string const & path)
{
    int err;

    file.open(path.c_str());
    if (file.good() == false)
        throw std::runtime_error("opening config_file failed");
    err = loadFile();
    if(err)
    {
        std::cerr << "Config file error at line: " << err << std::endl;
        throw std::runtime_error("Config file Error");
    }
}


int ServerList::loadFile()
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
        if(key == "#")
        {
            servers.push_back(Server());
            if(value.compare(key))
                servers.back().setHost(value);
        }
        else if(key == value)
        {
            std::cerr << "Value is empty" << std::endl;
            return(count);
        }
        else if(key == ">")
            servers.back().addLocation(value);
        else 
        {
            err = servers.back().setValue(key, value);
            if (err == -1)
                err = servers.back().setLastLocation(key, value);
            if(err)
                return(count);
        }
    }
    return(0);
}

void ServerList::print()const
{
    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        it->print();
    }
}

std::vector<Server> const & ServerList::getServers() const
{
    return servers;
}
