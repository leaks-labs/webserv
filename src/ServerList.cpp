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
        int sep = line.find(' ');
        std::string key = line.substr(0, sep);
        std::string value = line.substr(sep + 1, line.length());
        if (key.empty() || value.empty())
            continue;
        if (key == "#") {
            servers_.push_back(Server());
            if (value != key)
                servers_.back().set_host(value);
        } else if (key == value) {
            std::cerr << "Value is empty" << std::endl;
            return count;
        } else if (key == ">") {
            servers_.back().AddLocation(value);
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
    return 0;
}
