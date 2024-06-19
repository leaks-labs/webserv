#include "ListenerList.hpp"

#include <cstring>
#include <stdexcept>

const std::string  ListenerList::kDefaultPort = "8080";

ListenerList::ConstIterator::ConstIterator() : it_()
{
}

ListenerList::ConstIterator::ConstIterator(std::vector<Listener*>::const_iterator it) : it_(it)
{
}

ListenerList::ConstIterator::~ConstIterator()
{
}

ListenerList::ConstIterator::ConstIterator(const ConstIterator& src) : it_(src.it_)
{
}

ListenerList::ConstIterator&    ListenerList::ConstIterator::operator=(const ListenerList::ConstIterator& rhs) {
    it_ = rhs.it_;
    return *this;
}

Listener&   ListenerList::ConstIterator::operator*() {
    return **it_;
}

Listener*   ListenerList::ConstIterator::operator->() {
    return *it_;
}

ListenerList::ConstIterator&    ListenerList::ConstIterator::operator++() {
    ++it_;
    return *this;
}

ListenerList::ConstIterator ListenerList::ConstIterator::operator++(int) {
    ConstIterator    tmp = *this;
    ++it_;
    return tmp;
}

ListenerList::ConstIterator&    ListenerList::ConstIterator::operator--() {
    --it_;
    return *this;
}

ListenerList::ConstIterator ListenerList::ConstIterator::operator--(int) {
    ConstIterator    tmp = *this;
    --it_;
    return tmp;
}

bool    ListenerList::ConstIterator::operator==(const ConstIterator& rhs) const {
    return it_ == rhs.it_;
}

bool    ListenerList::ConstIterator::operator!=(const ConstIterator& rhs) const {
    return it_ != rhs.it_;
}

ListenerList::ListenerList()
{
    memset(&hints_, 0, sizeof(hints_));
    hints_.ai_family = PF_UNSPEC;
    hints_.ai_socktype = SOCK_STREAM;
    hints_.ai_flags = AI_PASSIVE;
}

ListenerList::~ListenerList()
{
    for (std::vector<struct addrinfo*>::iterator it = listener_records_.begin(); it != listener_records_.end(); ++it)
        freeaddrinfo(*it);
    for (std::vector<Listener*>::iterator it = EnabledListeners_.begin(); it != EnabledListeners_.end(); ++it)
        delete *it;
}

const Listener& ListenerList::operator[](size_t index) const {
    return *EnabledListeners_[index];
}

ListenerList::ConstIterator ListenerList::begin() const {
    return EnabledListeners_.begin();
}

ListenerList::ConstIterator ListenerList::end() const {
    return EnabledListeners_.end();
}

size_t  ListenerList::EnabledListenerCount() const
{
    return EnabledListeners_.size();
}

void    ListenerList::AddDefaultListenerRecords()
{
    AddListenerRecord(NULL, kDefaultPort);
}

void    ListenerList::AddListenerRecord(const char* ip, const std::string& port)
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

void    ListenerList::EnableListeners()
{
    for (std::vector<struct addrinfo*>::const_iterator it = listener_records_.begin(); it != listener_records_.end(); ++it)
        for (struct addrinfo *res = *it; res != NULL; res = res->ai_next)
            if (IsValidUniqAddr(*res) == true)
                listener_records_uniq_.push_back(res);
    for (std::vector<struct addrinfo*>::const_iterator it = listener_records_uniq_.begin(); it != listener_records_uniq_.end(); ++it) {
        Listener*   new_listener = new Listener(**it);
        try
        {
            EnabledListeners_.push_back(new_listener);
        }
        catch(const std::exception& e)
        {
            delete new_listener;
            throw;
        }
    }
}

bool    ListenerList::IsValidUniqAddr(const struct addrinfo& addr) const
{
    if (addr.ai_family != PF_INET6 && addr.ai_family != PF_INET)
        return false;
    for (std::vector<struct addrinfo*>::const_iterator it = listener_records_uniq_.begin(); it != listener_records_uniq_.end(); ++it)
        if (addr.ai_family == (*it)->ai_family && addr.ai_addrlen == (*it)->ai_addrlen && memcmp(addr.ai_addr, (*it)->ai_addr, addr.ai_addrlen) == 0)
            return false;
    return true;
}
