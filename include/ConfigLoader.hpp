#ifndef CONFIG_LOADER_HPP_
# define CONFIG_LOADER_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Config.hpp"

class ConfigLoader {

    private:
        std::ifstream & file;
        std::vector<Config> configs;
        ConfigLoader(const ConfigLoader& src);
        ConfigLoader&   operator=(const ConfigLoader& rhs);

    public:
        ConfigLoader(std::ifstream & file);
        ~ConfigLoader();
        int load(std::vector<Config> & configs);
};

#endif