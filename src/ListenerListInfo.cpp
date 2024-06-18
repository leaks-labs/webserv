#include "ListenerListInfo.hpp"

#include <cstring>
#include <stdexcept>

const std::string  ListenerListInfo::kDefaultPort = "8080";

ListenerListInfo::ListenerListInfo()
{
    memset(&hints_, 0, sizeof(hints_));
    hints_.ai_family = PF_UNSPEC;
    hints_.ai_socktype = SOCK_STREAM;
    hints_.ai_flags = AI_PASSIVE;
}

ListenerListInfo::~ListenerListInfo()
{
    for (std::vector<struct addrinfo*>::iterator it = listener_records_.begin(); it != listener_records_.end(); ++it)
        freeaddrinfo(*it);
}

void    ListenerListInfo::AddDefaultsRecords()
{
    AddRecords(NULL, kDefaultPort);
}

void    ListenerListInfo::AddRecords(const char* ip, const std::string& port)
{
    struct addrinfo *res;
    int             err;

    if ((err = getaddrinfo(ip, port.c_str(), &hints_, &res)) != 0)
        throw std::runtime_error("getaddrinfo(): " + std::string(gai_strerror(err)));
    try
    {
        listener_records_.push_back(res);
    }
    catch(const std::exception& e)
    {
        freeaddrinfo(res);
        throw;
    }
}

void    ListenerListInfo::CreateListeners(std::vector<Listener*>& listeners)
{
    for (std::vector<struct addrinfo*>::const_iterator it = listener_records_.begin(); it != listener_records_.end(); ++it)
        for (struct addrinfo *res = *it; res != NULL; res = res->ai_next)
            if (IsValidUniqAddr(res) == true)
                listeners_addr_uniq.push_back(res);
    for (std::vector<struct addrinfo*>::const_iterator it = listeners_addr_uniq.begin(); it != listeners_addr_uniq.end(); ++it) {
        Listener*   new_listener = new Listener(**it);
        try
        {
            listeners.push_back(new_listener);
        }
        catch(const std::exception& e)
        {
            delete new_listener;
            throw;
        }
    }
}


bool    ListenerListInfo::IsValidUniqAddr(struct addrinfo* addr) const
{
    if (addr->ai_family != PF_INET6 && addr->ai_family != PF_INET)
        return false;
    for (std::vector<struct addrinfo*>::const_iterator it = listeners_addr_uniq.begin(); it != listeners_addr_uniq.end(); ++it)
        if (addr->ai_family == (*it)->ai_family && memcmp(addr->ai_addr, (*it)->ai_addr, sizeof(struct sockaddr)) == 0)
            return false;
    return true;
}
