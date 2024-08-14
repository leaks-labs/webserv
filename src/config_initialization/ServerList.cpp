#include "ServerList.hpp"

ServerList& ServerList::Instance()
{
    static ServerList   instance;
    return instance;
}

Server& ServerList::operator[](size_t index)
{
    return servers_[index];
}

ServerList::Iterator    ServerList::Begin()
{
    return servers_.begin();
}

ServerList::Iterator    ServerList::End()
{
    return servers_.end();
}

ServerList::ConstIterator   ServerList::Begin() const
{
    return servers_.begin();
}

ServerList::ConstIterator   ServerList::End() const
{
    return servers_.end();
}

ServerList::ReverseIterator ServerList::Rbegin()
{
    return servers_.rbegin();
}

ServerList::ReverseIterator ServerList::Rend()
{
    return servers_.rend();
}

ServerList::ConstReverseIterator    ServerList::Rbegin() const
{
    return servers_.rbegin();
}

ServerList::ConstReverseIterator    ServerList::Rend() const
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
    ParseConfigFile(file);
}

int ServerList::IsSameAddr(const int acceptor_sfd, const struct addrinfo* addr_list) const
{
    struct sockaddr_storage addr_buf;
    socklen_t               len_buf = sizeof(addr_buf);
    if (getsockname(acceptor_sfd, reinterpret_cast<struct sockaddr*>(&addr_buf), &len_buf) == -1) {
        perror("ERROR: getsockname()");
        return -1;
    }
    for (const struct addrinfo *tmp = addr_list; tmp != NULL; tmp = tmp->ai_next)
        if (tmp->ai_addrlen == len_buf && memcmp(tmp->ai_addr, &addr_buf, len_buf) == 0)
            return 1;
    return 0;
}

const Server& ServerList::FindServer(const int acceptor_sfd, const std::string& name) const
{
    std::vector<const Server*>  matched;
    for (ServerList::ConstIterator it = servers_.begin(); it != servers_.end(); ++it) {
        int is_same_addr = IsSameAddr(acceptor_sfd, it->get_addr());
        if (is_same_addr == -1)
            throw std::runtime_error("IsSameAddr failed");
        if (is_same_addr == 1)
            matched.push_back(&(*it));
    }
    for (std::vector<const Server*>::const_iterator it = matched.begin(); it != matched.end(); ++it)
        if ((*it)->HasServerName(name))
            return **it;
    for (std::vector<const Server*>::const_iterator it = matched.begin(); it != matched.end(); ++it)
        if ((*it)->get_server_names()[0].empty())
            return **it;
    return *matched[0];
}

void    ServerList::Print() const
{
    for (std::vector<Server>::const_iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->Print();
}

ServerList::ServerList()
{
}

ServerList::~ServerList()
{
}

void    ServerList::ParseConfigFile(std::ifstream& file)
{
    int count = 1;
    try
    {
        bool    inside_location_block = false;
        for (std::string line; !file.eof() && std::getline(file, line) && !file.fail(); ++count) {
            if (line.empty())
                continue;
            std::string::size_type  sep = line.find(' ');
            std::string key = line.substr(0, sep);
            if (key.empty())
                throw std::runtime_error("key is empty");
            std::string value;
            if (sep != std::string::npos)
                value = line.substr(sep + 1, std::string::npos);
            if (key == "#" && (sep == std::string::npos || !value.empty())) {
                inside_location_block = false;
                servers_.push_back(Server());
                if (!value.empty())
                    servers_.back().set_host(value);
            } else if (servers_.empty()) {
                throw std::runtime_error("setting a value outside a server block is invalid");
            } else if (value.empty()) {
                throw std::runtime_error("value is empty");
            } else if (key == ">" || key == ">=") {
                inside_location_block = true;
                servers_.back().AddLocation(value);
                servers_.back().SetLastLocationStrict(key == ">=");
            } else if (servers_.back().SetValue(key, value) == Server::kValidKey) {
                if (inside_location_block)
                    throw std::runtime_error("this key is invalid inside a location block");
            } else if (servers_.back().SetLastLocation(key, value) == Server::kInvalidKey) {
                throw std::runtime_error("this key is invalid");
            }
        }
    }
    catch(const std::exception& e)
    {
        std::ostringstream  ss;
        ss << count;
        std::string err_str(ss.str());
        throw std::runtime_error("config file at line " + err_str + ": " + e.what());
    }
    if (file.fail() && !file.eof())
        throw std::runtime_error("while reading config file");
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it) {
        if (it->ServerNamesCount() > 1)
            it->PopDefaultServerName();
        it->RotateLocations();
    }
}
