#ifndef CONFIG_LOADER_HPP_
# define CONFIG_LOADER_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Server.hpp"

class ConfigLoader {

    private:
        std::ifstream & file;
        std::vector<Server*> configs;
        ConfigLoader(const ConfigLoader& src);
        ConfigLoader&   operator=(const ConfigLoader& rhs);

    public:
        ConfigLoader(std::ifstream & file);
        ~ConfigLoader();
        int loadFileConfig(std::vector<Server*> & configs);
        void print(std::vector<Server*> & servers)const;
};

#endif