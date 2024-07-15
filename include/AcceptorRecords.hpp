#ifndef ACCEPTOR_RECORDS_HPP_
# define ACCEPTOR_RECORDS_HPP_

# include <string>
# include <vector>

# include <netdb.h>

# include "ServerList.hpp"

class AcceptorRecords {
    public:
        AcceptorRecords();

        ~AcceptorRecords();

        void    InitAcceptors(ServerList& server_list);

    private:
        AcceptorRecords(const AcceptorRecords& src);
        AcceptorRecords&    operator=(const AcceptorRecords& rhs);

        const struct addrinfo*  AddAcceptorRecord(const char* ip, const std::string& port);
        void                    RegisterAcceptors();
        bool                    IsValidUniqAddr(const struct addrinfo& addr) const;

        struct addrinfo                 hints_;
        std::vector<struct addrinfo*>   acceptor_records_;
        std::vector<struct addrinfo*>   acceptor_records_uniq_;
};

#endif  // ACCEPTOR_RECORDS_HPP_
