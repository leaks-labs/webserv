#include "ConfigLoader.hpp"

ConfigLoader::ConfigLoader(std::ifstream & f) : file(f)
{};

ConfigLoader::~ConfigLoader(){};

int ConfigLoader::load(std::vector<Config*> & configs)
{
    std::string key;
    std::string value;
    int line;

    line = 0;
    while(file)
    {
        line++;
        std::getline(file, key, ' ');
        std::getline(file, value, '\n');
        if(key.length() == 0 || value.length() == 0)
            continue;
        if(!key.compare("#"))
            configs.push_back(new Config(value));
        else if(configs.back()->setValue(key, value))
        {
            std::cerr << "Error in config file at line " << line << std::endl;
            return(1);
        }
    }
    return(0);
}