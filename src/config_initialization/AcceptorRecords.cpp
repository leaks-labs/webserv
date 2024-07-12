#include "AcceptorRecords.hpp"

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "ConnectionAcceptor.hpp"
#include "InitiationDispatcher.hpp"

#include <iostream>

AcceptorRecords::AcceptorRecords()
{
    memset(&hints_, 0, sizeof(hints_));
    hints_.ai_family = PF_UNSPEC;
    hints_.ai_socktype = SOCK_STREAM;
    hints_.ai_flags = AI_PASSIVE;
}

AcceptorRecords::~AcceptorRecords()
{
    for (std::vector<struct addrinfo*>::iterator it = acceptor_records_.begin(); it != acceptor_records_.end(); ++it)
        freeaddrinfo(*it);
}

void    AcceptorRecords::InitAcceptors(ServerList& server_list)
{
    for (ServerList::Iterator it = server_list.Begin(); it != server_list.End(); ++it)
        it->set_addr(AddAcceptorRecord(it->get_host().c_str(), it->get_port()));
    RegisterAcceptors();
}

const struct addrinfo*  AcceptorRecords::AddAcceptorRecord(const char* ip, const std::string& port)
{
    struct addrinfo *addr;
    int             err;

    if ((err = getaddrinfo(ip, port.c_str(), &hints_, &addr)) != 0)
        throw std::runtime_error("getaddrinfo(): " + std::string(gai_strerror(err)));
    try
    {
        acceptor_records_.push_back(addr);
    }
    catch(const std::exception& e)
    {
        freeaddrinfo(addr);
        throw;
    }
    return addr;
}

void    AcceptorRecords::RegisterAcceptors()
{
    for (std::vector<struct addrinfo*>::const_iterator it = acceptor_records_.begin(); it != acceptor_records_.end(); ++it)
        for (struct addrinfo *addr = *it; addr != NULL; addr = addr->ai_next)
            if (IsValidUniqAddr(*addr))
                acceptor_records_uniq_.push_back(addr);
    for (std::vector<struct addrinfo*>::const_iterator it = acceptor_records_uniq_.begin(); it != acceptor_records_uniq_.end(); ++it) {
        ConnectionAcceptor* new_acceptor = NULL;
        try
        {
            new_acceptor = new ConnectionAcceptor(**it);
        }
        catch(const std::exception& e)
        {
            delete new_acceptor;
            throw;
        }
    }
}

bool    AcceptorRecords::IsValidUniqAddr(const struct addrinfo& addr) const
{
    if (addr.ai_family != PF_INET6 && addr.ai_family != PF_INET)
        return false;
    for (std::vector<struct addrinfo*>::const_iterator it = acceptor_records_uniq_.begin(); it != acceptor_records_uniq_.end(); ++it)
        if (addr.ai_family == (*it)->ai_family && addr.ai_addrlen == (*it)->ai_addrlen && memcmp(addr.ai_addr, (*it)->ai_addr, addr.ai_addrlen) == 0)
            return false;
    return true;
}
