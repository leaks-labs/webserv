#include "ServerList.hpp"

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

void    ServerList::AddServer()
{
    servers_.push_back(Server());
}

void    ServerList::OpenFile(const std::string& path)
{
    file_.open(path.c_str());
    if (file_.good() == false)
        throw std::runtime_error("opening config_file failed");
    int err = LoadFile();
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

int ServerList::LoadFile()
{
    int count = 1;
    for (std::string line; !file_.eof() && std::getline(file_, line) && !file_.fail(); ++count) {
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
            value = line.substr(sep + 1, line.length());
        if (key == "#" && (sep == std::string::npos || !value.empty())) {
            if(servers_.size() > 0)
                servers_.back().PopFirstLocation();
            servers_.push_back(Server());
            if (!value.empty())
                servers_.back().set_host(value);
        } else if (value.empty()) {
            std::cerr << "Value is empty" << std::endl;
            return count;
        } else if (key == ">" || key == ">=") {
            servers_.back().AddLocation(value);
            if(key == ">=")
                servers_.back().SetLastLocationStrict(true);
            else
                servers_.back().SetLastLocationStrict(false);
        } else {
            int err = servers_.back().SetValue(key, value);
            if (err == -1)
                err = servers_.back().SetLastLocation(key, value);
            if (err)
                return count;
        }
    }
    if (file_.fail() && !file_.eof()) {
        std::cerr << "Error reading file" << std::endl;
        return count;
    }
    if(servers_.size() > 0)
        servers_.back().PopFirstLocation();
    return 0;
}
