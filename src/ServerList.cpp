#include "ServerList.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

ServerList::ServerList()
{
}

ServerList::~ServerList()
{
}

Server& ServerList::operator[](size_t index)
{
    return servers_[index];
}

ServerList::Iterator    ServerList::begin()
{
    return servers_.begin();
}

ServerList::Iterator    ServerList::end()
{
    return servers_.end();
}

ServerList::ConstIterator   ServerList::begin() const
{
    return servers_.begin();
}

ServerList::ConstIterator   ServerList::end() const
{
    return servers_.end();
}

ServerList::ReverseIterator ServerList::rbegin()
{
    return servers_.rbegin();
}

ServerList::ReverseIterator ServerList::rend()
{
    return servers_.rend();
}

ServerList::ConstReverseIterator    ServerList::rbegin() const
{
    return servers_.rbegin();
}

ServerList::ConstReverseIterator    ServerList::rend() const
{
    return servers_.rend();
}

size_t  ServerList::Size() const
{
    return servers_.size();
}

void    ServerList::InitServerList(const std::string& path)
{
    std::ifstream   file(path.c_str());
    if (!file.good())
        throw std::runtime_error("opening config_file failed");
    int err = ParseConfigFile(file);
    if (err) {
        std::ostringstream  ss;
        ss << err;
        std::string err_str(ss.str());
        throw std::runtime_error("Config file Error at line: " + err_str);
    }
}

void    ServerList::Print() const
{
    for (std::vector<Server>::const_iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->Print();
}

const   std::vector<Server>&  ServerList::get_servers() const
{
    return servers_;
}

int ServerList::ParseConfigFile(std::ifstream& file)
{
    int count = 1;
    for (std::string line; !file.eof() && std::getline(file, line) && !file.fail(); ++count) {
        if (line.empty())
            continue;
        size_t  sep = line.find(' ');
        std::string key = line.substr(0, sep);
        if (key.empty()) {
            std::cerr << "Key is empty" << std::endl;
            return count;
        }
        std::string value;
        if (sep != std::string::npos)
            value = line.substr(sep + 1, std::string::npos);
        if (key == "#" && (sep == std::string::npos || !value.empty())) {
            servers_.push_back(Server());
            if (!value.empty())
                servers_.back().set_host(value);
        } else if (value.empty()) {
            std::cerr << "Value is empty" << std::endl;
            return count;
        } else if (key == ">" || key == ">=") {
            servers_.back().AddLocation(value);
            servers_.back().SetLastLocationStrict(key == ">=");
        } else {
            // If SetValue returns -1, the key is invalid. If it returns > 0, SetValue failed.
            // So, if SetValue returns -1, we try to set the value in the last location.
            // And if SetValue or SetLastLocation returns > 0, we return the line number as an error.
            int ret = servers_.back().SetValue(key, value);
            if (ret > 0 || (ret == -1 && servers_.back().SetLastLocation(key, value)))
                return count;
        }
    }
    if (file.fail() && !file.eof()) {
        std::cerr << "Error reading file" << std::endl;
        return count;
    }
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it) {
        if (it->LocationsCount() > 1)
            it->PopFirstLocation();
        if (it->ServerNamesCount() > 1)
            it->PopFirstServerNames();
    }
    return 0;
}
